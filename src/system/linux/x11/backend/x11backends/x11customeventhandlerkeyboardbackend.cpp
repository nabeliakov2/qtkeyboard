#include "system/linux/x11/backend/x11backends/x11customeventhandlerkeyboardbackend.h"

#ifdef Q_OS_LINUX
#include <QtCore/QSet>
#include <QtCore/QTimer>
#include <QtCore/QPointer>
#include <QtCore/QDebug>

#include <QtX11Extras/QX11Info>

#include <X11/Xlib.h>   // XGetKeyboardMapping, XFree

#include "core/utilities/keyboardutility.h"
#include "core/backend/eventhandlers/abstractkeyboardeventhandler.h"

#include "system/linux/x11/backend/keycodetotextconverting/utilities/keysym2ucs.h" // keysym2ucs


namespace qt_keyboard
{

struct X11CustomEventHandlerKeyboardBackend::X11HandlerBackendPrivate Q_DECL_FINAL // private
{
    QPointer<AbstractKeyboardEventHandler> pEventHandler;

    X11HandlerBackendPrivate(AbstractKeyboardEventHandler *const _pEventHandler)
        : pEventHandler(_pEventHandler) {}
    ~X11HandlerBackendPrivate() Q_DECL_NOEXCEPT = default;

    X11HandlerBackendPrivate(const X11HandlerBackendPrivate &_other) = delete;
    X11HandlerBackendPrivate(X11HandlerBackendPrivate &&_other) = delete;
    X11HandlerBackendPrivate &operator=(const X11HandlerBackendPrivate &_other) = delete;
    X11HandlerBackendPrivate &operator=(X11HandlerBackendPrivate &&_other) = delete;
};

X11CustomEventHandlerKeyboardBackend::X11CustomEventHandlerKeyboardBackend(AbstractKeyboardEventHandler *const _pEventHandler,
                                                                           QObject *_parent /* = Q_NULLPTR */) // explicit public
    : X11KeyboardBackend(_parent),
      pX11HandlerBackendPrivate(new X11HandlerBackendPrivate(_pEventHandler))
{
    init();
}

X11CustomEventHandlerKeyboardBackend::~X11CustomEventHandlerKeyboardBackend() Q_DECL_NOEXCEPT // virtual public override final
{}

void X11CustomEventHandlerKeyboardBackend::init()  // private
{
    Q_ASSERT(pX11HandlerBackendPrivate);

    if (!pX11HandlerBackendPrivate -> pEventHandler)
    {
        throw std::invalid_argument(std::string("pEventHandler cannot be null in ") + Q_FUNC_INFO);
    }

    if (!connect(pX11HandlerBackendPrivate -> pEventHandler.data(), &AbstractKeyboardEventHandler::started,
                 this, &X11CustomEventHandlerKeyboardBackend::updateInitialized))
    {
        throw std::runtime_error(std::string("Signal started() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pX11HandlerBackendPrivate -> pEventHandler.data(), &AbstractKeyboardEventHandler::layoutsEvent,
                 this, &X11CustomEventHandlerKeyboardBackend::updateLayouts))   // &AbstractKeyboardBackend::updateLayouts causes "protected" compiler error here
    {
        throw std::runtime_error(std::string("Signal layoutsEvent() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pX11HandlerBackendPrivate -> pEventHandler.data(), &AbstractKeyboardEventHandler::currentLayoutEvent,
                 this, &X11CustomEventHandlerKeyboardBackend::updateCurrentLayout))

    {
        throw std::runtime_error(std::string("Signal currentLayoutEvent() connection error in ") + Q_FUNC_INFO);
    }

    if (!connect(pX11HandlerBackendPrivate -> pEventHandler.data(), &AbstractKeyboardEventHandler::locksStateEvent,
                 this, &X11CustomEventHandlerKeyboardBackend::updateLockState))
    {
        throw std::runtime_error(std::string("Signal indicatorsStateEvent() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pX11HandlerBackendPrivate -> pEventHandler.data(), &AbstractKeyboardEventHandler::keyStateEvent,
                 this, &X11CustomEventHandlerKeyboardBackend::updateKeyState))
    {
        throw std::runtime_error(std::string("Signal indicatorsStateEvent() connection error in ") + Q_FUNC_INFO);
    }

// Schehule event handler for starting
// Must be in the event loop!
    QTimer::singleShot(0, pX11HandlerBackendPrivate -> pEventHandler.data(),
                       &AbstractKeyboardEventHandler::start);
}

} // qt_keyboard


#endif // Q_OS_LINUX

