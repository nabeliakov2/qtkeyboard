#ifndef X11EVENTHANDLERSUPPORTUTILITY_H
#define X11EVENTHANDLERSUPPORTUTILITY_H

#include <QtGlobal>

#ifdef Q_OS_LINUX

#include <QtCore/QMetaType>

#include <X11/Xlib.h>   // a lot of Xlib calls

#include "core/utilities/keyboardutility.h"

class QStringList;

namespace qt_keyboard
{

class X11EventHandlerSupportUtility Q_DECL_FINAL
{
public:
    static const int CapsLockInt {1};
    static const int NumLockInt {2};
    static const int ScrollLockInt {4};

    static Display *connectToControlDisplay(int *const _eventReturn, char *const _displayName = Q_NULLPTR);
    static Display *connectToDataDisplay(const char *const _displayName = Q_NULLPTR);

    static QStringList getLayouts(Display *const _display);
    static int getCurrentLayout(Display *const _display);
    static KeyboardUtility::LocksStateFlags lockStateFromSystemFlags(const int _sysFlags);
    static KeyboardUtility::LocksStateFlags getCurrentLockstate(Display *const _display);

private:
    X11EventHandlerSupportUtility() = delete;
    ~X11EventHandlerSupportUtility() = delete;
};

} // qt_keyboard

#endif // Q_OS_LINUX

#endif // X11EVENTHANDLERSUPPORTUTILITY_H
