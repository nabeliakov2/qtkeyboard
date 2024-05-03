#include "system/linux/x11/backend/eventhandlers/x11xrecordxkbdeventhandler.h"

#ifdef Q_OS_LINUX
#include <QtCore/QPointer>
#include <QtCore/QDebug>

#include <X11/Xlibint.h> // XRecordDatum field types
#include <X11/XKBlib.h> // Xkb*
#include <X11/extensions/record.h> // XRecord*

#include "system/linux/x11/backend/eventhandlers/utilities/x11eventhandlersupportutility.h"

namespace qt_keyboard
{

struct X11XRecordXKbdEventHandler::HandlerPrivate Q_DECL_FINAL // private
{
    union XRecordDatum // libxnee
    {
        unsigned char    type;
        xEvent           event;
        xResourceReq     req;
        xGenericReply    reply;
        xError           error;
        xConnSetupPrefix setup;
    } *xRecordDatum {nullptr};

    QByteArray displayName;

    std::atomic_bool isRunning {false};

    HandlerPrivate() = default;
    HandlerPrivate(const QByteArray &_displayName)
        : displayName(_displayName) {}
    ~HandlerPrivate() = default;

    HandlerPrivate(const HandlerPrivate &_other) = delete;
    HandlerPrivate(HandlerPrivate &&_other) = delete;
    HandlerPrivate &operator=(const HandlerPrivate &_other) = delete;
    HandlerPrivate &operator=(HandlerPrivate &&_other) = delete;
};

/// Library standard
class XRecordInterceptProcHandler Q_DECL_FINAL
{
public:
///! callback should be called from the corresponding X11EventHandler thread
    static void callback(XPointer priv, XRecordInterceptData *hook)
    {
        X11XRecordXKbdEventHandler *const x11EventHandler = reinterpret_cast<X11XRecordXKbdEventHandler*>(priv);
        if (hook -> category == XRecordFromServer)
        {
            Q_ASSERT(x11EventHandler -> pHandlerPrivate);

            x11EventHandler -> pHandlerPrivate -> xRecordDatum = (X11XRecordXKbdEventHandler::HandlerPrivate::XRecordDatum*)hook->data;
            x11EventHandler -> handleCapturedEvent();
        }
        else
        {
            XRecordFreeData(hook);
        }
    }
};

X11XRecordXKbdEventHandler::X11XRecordXKbdEventHandler(QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractKeyboardEventHandler(_parent),
      pHandlerPrivate(new HandlerPrivate)
{}

X11XRecordXKbdEventHandler::X11XRecordXKbdEventHandler(const QByteArray &_displayName,
                                                       QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractKeyboardEventHandler(_parent),
      pHandlerPrivate(new HandlerPrivate(_displayName))
{}


X11XRecordXKbdEventHandler::~X11XRecordXKbdEventHandler() Q_DECL_NOEXCEPT // virtual public override final
{}

void X11XRecordXKbdEventHandler::start()    // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    pHandlerPrivate -> isRunning.store(true, std::memory_order_release);
    run();
}

void X11XRecordXKbdEventHandler::stop() // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    pHandlerPrivate -> isRunning.store(false, std::memory_order_release);
}

bool X11XRecordXKbdEventHandler::isRunning() const  // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    return pHandlerPrivate -> isRunning.load(std::memory_order_acquire);
}

void X11XRecordXKbdEventHandler::run() // private
{
    Q_ASSERT(pHandlerPrivate);

    int xkbEventType;
    Display *const controlDisplay =
            X11EventHandlerSupportUtility::connectToControlDisplay(&xkbEventType, pHandlerPrivate -> displayName.data());
    Display *const dataDisplay =
            X11EventHandlerSupportUtility::connectToDataDisplay(pHandlerPrivate -> displayName.data());

    if (!controlDisplay || !dataDisplay)
    {
        throw std::runtime_error("Failed to open control or data display for XRecord");
    }

    try
    {
        XSynchronize(controlDisplay, True);

        // initial setup
        const QStringList l_layouts(X11EventHandlerSupportUtility::getLayouts(controlDisplay));
        emit layoutsEvent(l_layouts);

        const int l_currentLayout = X11EventHandlerSupportUtility::getCurrentLayout(controlDisplay);
        emit currentLayoutEvent(l_currentLayout);

        const KeyboardUtility::LocksStateFlags l_lockState(X11EventHandlerSupportUtility::getCurrentLockstate(controlDisplay));
        emit locksStateEvent(l_lockState);

        unsigned long xkbEventMask = XkbStateNotifyMask | XkbIndicatorStateNotifyMask;
        XkbSelectEvents(controlDisplay, XkbUseCoreKbd, xkbEventMask, xkbEventMask);

        int major, minor;
        if (!XRecordQueryVersion(controlDisplay, &major, &minor))
        {
            throw std::runtime_error("XRecord extension not supported on this X Server");
        }
        XRecordRange *xRecordRange[1];
        xRecordRange[0] = XRecordAllocRange();
        if (!xRecordRange[0])
        {
            throw std::runtime_error("Failed to alloc XRecord range object");
        }

        try
        {
            xRecordRange[0] -> device_events.first = KeyPress;
            xRecordRange[0] -> device_events.last = KeyRelease;
            XRecordClientSpec xRecordClientSpec = XRecordAllClients;
            const XRecordContext xRecordContext = XRecordCreateContext(controlDisplay,
                                                                       0, &xRecordClientSpec, 1, xRecordRange, 1);
            if (!xRecordContext)
            {
                throw std::runtime_error("Failed to create XRecord context");
            }

            try
            {
                if (!XRecordEnableContextAsync(dataDisplay, xRecordContext,
                                               XRecordInterceptProcHandler::callback,
                                               (char*)this))
                {
                    throw std::runtime_error("Failed to enable XRecord context");
                }

                try
                {
                    const int controlFd = XConnectionNumber(controlDisplay);
                    const int dataFd = XConnectionNumber(dataDisplay);

                    emit started();

                    while (pHandlerPrivate -> isRunning.load(std::memory_order_acquire))
                    {
                        fd_set fdSet;
                        FD_ZERO(&fdSet);
                        FD_SET(controlFd, &fdSet);
                        FD_SET(dataFd, &fdSet);
                        timeval timeout;
                        timeout.tv_sec = 2;
                        timeout.tv_usec = 0;
                        select(qMax(controlFd, dataFd) + 1, &fdSet, NULL, NULL, &timeout);
                        if (FD_ISSET(dataFd, &fdSet))
                        {
                            XRecordProcessReplies(dataDisplay);
                        }
                        if (FD_ISSET(controlFd, &fdSet))
                        {
                            XEvent l_event;
                            XNextEvent(controlDisplay, &l_event);
                            if (l_event.type == xkbEventType)
                            {
                                XkbEvent *const xkbEvent = reinterpret_cast<XkbEvent*>(&l_event);
                                switch (xkbEvent -> any.xkb_type)
                                {
                                case XkbStateNotify:
                                    if (xkbEvent -> state.changed & XkbGroupStateMask)
                                    {
                                        const int l_currentEvLayout = xkbEvent -> state.group;
                                        emit currentLayoutEvent(l_currentEvLayout);
                                    }
                                    break;
                                case XkbIndicatorStateNotify:
                                    const KeyboardUtility::LocksStateFlags l_evLockState(
                                                X11EventHandlerSupportUtility::lockStateFromSystemFlags(xkbEvent -> indicators.state));
                                    emit locksStateEvent(l_evLockState);
                                    break;
                                } // switch
                            } // -> if (l_event.type == pHandlerPrivate -> xkbEventType)
                        } // -> if (FD_ISSET(controlFd, &fdSet))
                    } // -> while (pHandlerPrivate -> b_isRunning)
                    XRecordDisableContext(controlDisplay, xRecordContext);
                    XRecordFreeContext(controlDisplay, xRecordContext);
                    XFree(xRecordRange[0]);
                    XCloseDisplay(dataDisplay);
                    XCloseDisplay(controlDisplay);

                    emit finished();
                } // try X event loop
                catch (...)
                {
                    XRecordDisableContext(controlDisplay, xRecordContext);
                    throw;
                }
            } // try
            catch (...)
            {
                XRecordFreeContext(controlDisplay, xRecordContext);
                throw;
            }
        } // try
        catch (...)
        {
            XFree(xRecordRange[0]);
            throw;
        }
    } // try
    catch (...)
    {
        XCloseDisplay(dataDisplay);
        XCloseDisplay(controlDisplay);
        throw;
    }
}

void X11XRecordXKbdEventHandler::handleCapturedEvent() // private
{
    Q_ASSERT(pHandlerPrivate);

    switch (pHandlerPrivate -> xRecordDatum -> type)
    {
        case KeyPress:
        {
            const int keycode = pHandlerPrivate -> xRecordDatum -> event.u.u.detail;
            emit keyStateEvent(keycode, true);  // auto synced
            break;
        }
        case KeyRelease:
        {
            const int keycode = pHandlerPrivate -> xRecordDatum -> event.u.u.detail;

            emit keyStateEvent(keycode, false); // auto synced
/*
// WORKAROUND!
            if (keycode == CAPS_KEY_CODE_X)
            {
                forceUpdateCaps();
            }*/
            break;
        }
    }
}


} // qt_keyboard

#endif // Q_OS_LINUX

