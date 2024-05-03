#ifndef KEYBOARDUTILITY_H
#define KEYBOARDUTILITY_H

#include <QtCore/QFlags>
#include <QtCore/QMetaType>

namespace qt_keyboard
{

using LayoutNameMapper = QHash<QString, QString>;

class KeyboardUtility Q_DECL_FINAL
{
public:
    static const int UserKeys = 1000;

    enum SpecialKeycodes {
        LeftCtrl = 37,
        RightCtrl = 105,
        LeftShift = 50,
        RightShift = 62,
        LeftAlt = 64,
        RightAlt = 108,
        CapsLock = 66,
        NumLock = 77,
        ScrollLock = 78,
        LeftMeta = 133,
        Space = 65,
        Enter = 36,
        SpacerStretch = UserKeys, // 1000
        SwitchLayout,   // 1001
        Close   // 1002
    };

    enum LocksStateFlag {
        NoLocksFlag = 0x0,
        CapsLockFlag = 0x1,
        NumLockFlag = 0x2,
        ScrollLockFlag = 0x4
    };
    Q_DECLARE_FLAGS(LocksStateFlags, LocksStateFlag)

    static const QSet<int> ModifierSet;
    static const int processEventsMaxTime = 5000; // ms
    static const int asyncCheckInterval = 200; // ms


private:
    KeyboardUtility() = delete;
    ~KeyboardUtility() = delete;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(KeyboardUtility::LocksStateFlags)


} // qt_keyboard

Q_DECLARE_METATYPE(qt_keyboard::KeyboardUtility::LocksStateFlags)

#endif // KEYBOARDUTILITY_H
