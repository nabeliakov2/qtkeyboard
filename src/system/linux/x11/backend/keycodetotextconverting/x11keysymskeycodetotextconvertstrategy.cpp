#include "system/linux/x11/backend/keycodetotextconverting/x11keysymskeycodetotextconvertstrategy.h"

#ifdef Q_OS_LINUX
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <QtX11Extras/QX11Info>

#include <X11/Xlib.h>   // XGetKeyboardMapping, XFree

#include "core/backend/keycodetotextconverting/utilities/keycodetotextconvertutility.h"

#include "system/linux/x11/backend/keycodetotextconverting/utilities/keysym2ucs.h" // keysym2ucs



namespace qt_keyboard
{

const QString InvalidText("\uFFFF");

QString X11KeySymsKeycodeToTextConvertStrategy::keyText(const int _keycode,
                                                        const int _layoutIndex,
                                                        const bool _shift /* = false */,
                                                        const bool _caps /* = false */) const  // virtual public override final
{
    int keySymCount {0};
    KeySym *const keySym = XGetKeyboardMapping(QX11Info::display(), _keycode, 1, &keySymCount);


    const int normalIndex = _layoutIndex << 1;
    const int shiftedIndex = normalIndex + 1;
    const KeySym normalKeySym = keySym[normalIndex];
    const KeySym shiftedKeySym = keySym[shiftedIndex];

    long ucs = keysym2ucs(_shift ? shiftedKeySym : normalKeySym);
    QString text(QChar((uint)ucs));
    capsTransform(text, _shift, _caps);

    XFree((char*)keySym);
    return text;
}

QString X11KeySymsKeycodeToTextConvertStrategy::keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                           const bool _shift /* = false */, const bool _caps /* = false */) const // virtual public override final
{
    const QString _text(keyText(_keycode, _layoutIndex, _shift, _caps));
    _hasKeyTextReturn = _text != InvalidText;
    return _hasKeyTextReturn ? _text : QString();
}

bool X11KeySymsKeycodeToTextConvertStrategy::hasKeyText(const int _keycode, const int _layoutIndex, const bool _shift /* = false */) const // virtual public override final
{
    const QString _text(keyText(_keycode, _layoutIndex, _shift));
    return _text != InvalidText;
}


} // qt_keyboard

#endif // Q_OS_LINUX
