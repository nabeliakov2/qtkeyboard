#ifndef X11KEYSYMSKEYCODETOTEXTCONVERTSTRATEGY_H
#define X11KEYSYMSKEYCODETOTEXTCONVERTSTRATEGY_H

#include "core/backend/keycodetotextconverting/keycodetotextconvertstrategyinterface.h"

#ifdef Q_OS_LINUX

namespace qt_keyboard
{

class X11KeySymsKeycodeToTextConvertStrategy Q_DECL_FINAL : public KeycodeToTextConvertStrategyInterface
{
public:
    X11KeySymsKeycodeToTextConvertStrategy() = default;
    virtual ~X11KeySymsKeycodeToTextConvertStrategy() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL = default;

    X11KeySymsKeycodeToTextConvertStrategy(const X11KeySymsKeycodeToTextConvertStrategy &_other) = default;
    X11KeySymsKeycodeToTextConvertStrategy(X11KeySymsKeycodeToTextConvertStrategy &&_other) = default;

    X11KeySymsKeycodeToTextConvertStrategy &operator=(const X11KeySymsKeycodeToTextConvertStrategy &_other) = default;
    X11KeySymsKeycodeToTextConvertStrategy &operator=(X11KeySymsKeycodeToTextConvertStrategy &&_other) = default;

    virtual QString keyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QString keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                   const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool hasKeyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;


};

} // qt_keyboard

#endif // Q_OS_LINUX

#endif // X11KEYSYMSKEYCODETOTEXTCONVERTSTRATEGY_H
