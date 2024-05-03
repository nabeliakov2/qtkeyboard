#include "system/linux/x11/backend/eventhandlers/abstractx11xkbdqueryingeventhandler.h"

#ifdef Q_OS_LINUX

#include <QtCore/QSet>

#include "system/linux/x11/backend/eventhandlers/utilities/x11eventhandlersupportutility.h"

namespace qt_keyboard
{

namespace
{
QSet<int> getPressedKeys(Display *const _display);

} // namespace

struct AbstractX11XKbdQueryingEventHandler::AbstractHandlerPrivate Q_DECL_FINAL // private
{
    QByteArray displayName;

    Display *controlDisplay {Q_NULLPTR};
    QStringList layouts;
    int currentLayout;
    KeyboardUtility::LocksStateFlags lockState;
    QSet<int> pressedKeys;

    AbstractHandlerPrivate() = default;
    explicit AbstractHandlerPrivate(const QByteArray &_displayName)
        : displayName(_displayName) {}
    ~AbstractHandlerPrivate() = default;

    AbstractHandlerPrivate(const AbstractHandlerPrivate &_other) = delete;
    AbstractHandlerPrivate(AbstractHandlerPrivate &&_other) = delete;
    AbstractHandlerPrivate &operator=(const AbstractHandlerPrivate &_other) = delete;
    AbstractHandlerPrivate &operator=(AbstractHandlerPrivate &&_other) = delete;
};


AbstractX11XKbdQueryingEventHandler::AbstractX11XKbdQueryingEventHandler(QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractKeyboardEventHandler(_parent),
      pAbstractHandlerPrivate(new AbstractHandlerPrivate)
{}

AbstractX11XKbdQueryingEventHandler::AbstractX11XKbdQueryingEventHandler(const QByteArray &_displayName,
                                                                         QObject *_parent /* = Q_NULLPTR */)  // public
    : AbstractKeyboardEventHandler(_parent),
      pAbstractHandlerPrivate(new AbstractHandlerPrivate(_displayName))
{}

AbstractX11XKbdQueryingEventHandler::~AbstractX11XKbdQueryingEventHandler() Q_DECL_NOEXCEPT // virtual public override
{
    AbstractX11XKbdQueryingEventHandler::stop();
}

void AbstractX11XKbdQueryingEventHandler::stop() // virtual public override
{
    Q_ASSERT(pAbstractHandlerPrivate);

    if (pAbstractHandlerPrivate -> controlDisplay)
    {
        XCloseDisplay(pAbstractHandlerPrivate -> controlDisplay);
        pAbstractHandlerPrivate -> controlDisplay = Q_NULLPTR;
    }
}

void AbstractX11XKbdQueryingEventHandler::run() // protected
{
    Q_ASSERT(pAbstractHandlerPrivate);

    int xkbEventType;
    pAbstractHandlerPrivate -> controlDisplay =
            X11EventHandlerSupportUtility::connectToControlDisplay(&xkbEventType, pAbstractHandlerPrivate -> displayName.data());

    if (!pAbstractHandlerPrivate -> controlDisplay)
    {
        throw std::runtime_error("Failed to open control display");
    }

    try
    {
        XSynchronize(pAbstractHandlerPrivate -> controlDisplay, True);

        // initial setup
        const QStringList l_layouts(X11EventHandlerSupportUtility::getLayouts(pAbstractHandlerPrivate -> controlDisplay));
        pAbstractHandlerPrivate -> layouts = l_layouts;
        emit layoutsEvent(l_layouts);

        const int l_currentLayout = X11EventHandlerSupportUtility::getCurrentLayout(pAbstractHandlerPrivate -> controlDisplay);
        pAbstractHandlerPrivate -> currentLayout = l_currentLayout;
        emit currentLayoutEvent(l_currentLayout);

        const KeyboardUtility::LocksStateFlags l_lockState(
                    X11EventHandlerSupportUtility::getCurrentLockstate(pAbstractHandlerPrivate -> controlDisplay));
        pAbstractHandlerPrivate -> lockState = l_lockState;
        emit locksStateEvent(l_lockState);

        emit started();
        doRun();

    } // try
    catch (...)
    {
        XCloseDisplay(pAbstractHandlerPrivate -> controlDisplay);
        throw;
    }
}

void AbstractX11XKbdQueryingEventHandler::poll()    // protected
{
    Q_ASSERT(pAbstractHandlerPrivate);

    const QStringList l_layouts(X11EventHandlerSupportUtility::getLayouts(pAbstractHandlerPrivate -> controlDisplay));
    if (l_layouts != pAbstractHandlerPrivate -> layouts)
    {
        pAbstractHandlerPrivate -> layouts = l_layouts;
        emit layoutsEvent(pAbstractHandlerPrivate -> layouts);
    }

    const int l_currentLayout = X11EventHandlerSupportUtility::getCurrentLayout(pAbstractHandlerPrivate -> controlDisplay);
    if (l_currentLayout != pAbstractHandlerPrivate -> currentLayout)
    {
        pAbstractHandlerPrivate -> currentLayout = l_currentLayout;
        emit currentLayoutEvent(pAbstractHandlerPrivate -> currentLayout);
    }

    const KeyboardUtility::LocksStateFlags l_lockState(
                X11EventHandlerSupportUtility::getCurrentLockstate(pAbstractHandlerPrivate -> controlDisplay));
    if (l_lockState != pAbstractHandlerPrivate -> lockState)
    {
        pAbstractHandlerPrivate -> lockState = l_lockState;
        emit locksStateEvent(pAbstractHandlerPrivate -> lockState);
    }

    const QSet<int> l_pressedKeys(getPressedKeys(pAbstractHandlerPrivate -> controlDisplay));

    if (l_pressedKeys != pAbstractHandlerPrivate -> pressedKeys)
    {
        const QSet<int> wasPressed(l_pressedKeys - pAbstractHandlerPrivate -> pressedKeys);
        const QSet<int> wasReleased(pAbstractHandlerPrivate -> pressedKeys - l_pressedKeys);
        pAbstractHandlerPrivate -> pressedKeys = l_pressedKeys;
        for (const int k : wasPressed)
        {
            emit keyStateEvent(k, true);
        }
        for (const int k : wasReleased)
        {
            emit keyStateEvent(k, false);
        }
    }
}

namespace
{

QSet<int> getPressedKeys(Display *const _display)
{
    static const uint32_t K_KEYMAP_SIZE = 32;
    static const uint32_t L_BYTE = 8;

    Q_ASSERT(_display);

    char keyMap[K_KEYMAP_SIZE];
    XQueryKeymap(_display, keyMap);

    QSet<int> pressedKeys;
    for (uint32_t i = 0; i < K_KEYMAP_SIZE; ++i)
    {
        const char &currentChar = keyMap[i];
        for (uint32_t j = 0; j < L_BYTE; ++j)
        {
            const bool isKeyPressed = ((1 << j) & currentChar) != 0;
            if (isKeyPressed)
            {
                int keyCode = (i * sizeof(char) * 8) + j;
                pressedKeys.insert(keyCode);
            }
        }
    }
    return pressedKeys;
}

}   // namespace



} // qt_keyboard

#endif // Q_OS_LINUX
