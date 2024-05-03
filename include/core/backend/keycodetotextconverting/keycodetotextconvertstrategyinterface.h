#ifndef KEYCODETOTEXTCONVERTSTRATEGYINTERFACE_H
#define KEYCODETOTEXTCONVERTSTRATEGYINTERFACE_H

#include <QtGlobal>

class QString;

namespace qt_keyboard
{

class KeycodeToTextConvertStrategyInterface
{
public:
    KeycodeToTextConvertStrategyInterface() = default;
    virtual ~KeycodeToTextConvertStrategyInterface() Q_DECL_NOEXCEPT = default;

    KeycodeToTextConvertStrategyInterface(const KeycodeToTextConvertStrategyInterface &_other) = default;
    KeycodeToTextConvertStrategyInterface(KeycodeToTextConvertStrategyInterface &&_other) = default;

    KeycodeToTextConvertStrategyInterface &operator=(const KeycodeToTextConvertStrategyInterface &_other) = default;
    KeycodeToTextConvertStrategyInterface &operator=(KeycodeToTextConvertStrategyInterface &&_other) = default;

    virtual bool hasKeyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false) const = 0;
    virtual QString keyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false, const bool _caps = false) const = 0;
    virtual QString keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                   const bool _shift = false, const bool _caps = false) const = 0;
};



} // qt_keyboard

#endif // KEYCODETOTEXTCONVERTSTRATEGYINTERFACE_H
