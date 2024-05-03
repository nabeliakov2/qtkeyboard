#include "system/linux/x11/backend/eventhandlers/utilities/x11eventhandlersupportutility.h"

#ifdef Q_OS_LINUX

#include <QtCore/QStringList>

#include <X11/XKBlib.h> // Xkb*

namespace qt_keyboard
{

Display *X11EventHandlerSupportUtility::connectToControlDisplay(int *const _eventReturn, char *const _displayName /* = Q_NULLPTR */) // static public
{
    int errorReturn;
    int majorVer = XkbMajorVersion;
    int minorVer = XkbMinorVersion;
    int reasonReturn;

    Display *const display =
            XkbOpenDisplay(_displayName,
                           _eventReturn, &errorReturn, &majorVer, &minorVer, &reasonReturn);
    switch (reasonReturn)
    {
        case XkbOD_BadLibraryVersion:
            throw std::runtime_error("Bad XKB library version.");
        case XkbOD_ConnectionRefused:
            throw std::runtime_error("Connection to X server refused.");
        case XkbOD_BadServerVersion:
            throw std::runtime_error("Bad X11 server version.");
        case XkbOD_NonXkbServer:
            throw std::runtime_error("XKB not present.");
        case XkbOD_Success:
            break;
    }
    return display;
}

Display *X11EventHandlerSupportUtility::connectToDataDisplay(const char *const _displayName /* = Q_NULLPTR */)  // static public
{
    return XOpenDisplay(_displayName);
}

QStringList X11EventHandlerSupportUtility::getLayouts(Display *const _display)  // static public
{
    Q_ASSERT(_display);

    XkbDescRec xkbDescRec = {};
    xkbDescRec.device_spec = XkbUseCoreKbd;
    Status status = XkbGetControls(_display, XkbGroupsWrapMask, &xkbDescRec);
    if (status != Success)
    {
        throw std::runtime_error(std::string("XkbGetControls error :") + std::to_string(status));
    }
    status = XkbGetNames(_display, XkbGroupNamesMask, &xkbDescRec);
    if (status != Success)
    {
        XkbFreeControls(&xkbDescRec, XkbGroupsWrapMask, True);

        throw std::runtime_error(std::string("XkbGetNames error :") + std::to_string(status));
    }
    if (!xkbDescRec.ctrls || !xkbDescRec.ctrls -> num_groups)
    {
        XkbFreeControls(&xkbDescRec, XkbGroupsWrapMask, True);
        XkbFreeNames(&xkbDescRec, XkbGroupNamesMask, True);

        throw std::runtime_error(std::string("Groups not found"));
    }
    QStringList resList;
    for (int i = 0; i < xkbDescRec.ctrls -> num_groups; ++i)
    {
        char *const layoutNameNTS = XGetAtomName(_display, xkbDescRec.names -> groups[i]);
        if (layoutNameNTS)
        {
            const QString layoutName(layoutNameNTS);
            if (!layoutName.isEmpty())
            {
                resList.append(layoutName);
            }
            XFree(layoutNameNTS);
        }
    }
    return resList;
}

int X11EventHandlerSupportUtility::getCurrentLayout(Display *const _display)    // static public
{
    Q_ASSERT(_display);

    XkbStateRec xkbState;
    if (XkbGetState(_display, XkbUseCoreKbd, &xkbState) != Success)
    {
        throw std::runtime_error(std::string("Get keyboard state error in ") + Q_FUNC_INFO);
    }
    return xkbState.group;
}

KeyboardUtility::LocksStateFlags X11EventHandlerSupportUtility::lockStateFromSystemFlags(const int _sysFlags)   // static public
{
    KeyboardUtility::LocksStateFlags resFlags {0x0};
    if (_sysFlags & CapsLockInt)
    {
        resFlags |= KeyboardUtility::CapsLockFlag;
    }
    if (_sysFlags & NumLockInt)
    {
        resFlags |= KeyboardUtility::NumLockFlag;
    }
    if (_sysFlags & ScrollLockInt)
    {
        resFlags |= KeyboardUtility::ScrollLockFlag;
    }
    return resFlags;
}

KeyboardUtility::LocksStateFlags X11EventHandlerSupportUtility::getCurrentLockstate(Display *const _display)    // static public
{
    Q_ASSERT(_display);

    unsigned int n;
    if (XkbGetIndicatorState(_display, XkbUseCoreKbd, &n) != Success)
    {
        throw std::runtime_error(std::string("Get indicators state error in ") + Q_FUNC_INFO);
    }
    return lockStateFromSystemFlags(n);
}




} // qt_keyboard


#endif // Q_OS_LINUX
