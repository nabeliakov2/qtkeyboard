#ifndef KEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H
#define KEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H

#include <memory>

#include "core/backend/keycodetotextconverting/keycodetotextconvertstrategyinterface.h"
#include "core/backend/keycodetotextconverting/utilities/keycodetotextconvertutility.h"

namespace qt_keyboard
{

class KeycodeToTextConvertMappingStrategy : public KeycodeToTextConvertStrategyInterface
{
public:
    KeycodeToTextConvertMappingStrategy();
    virtual ~KeycodeToTextConvertMappingStrategy() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE;

    KeycodeToTextConvertMappingStrategy(const KeycodeToTextConvertMappingStrategy &_other);
    KeycodeToTextConvertMappingStrategy(KeycodeToTextConvertMappingStrategy &&_other);

    KeycodeToTextConvertMappingStrategy &operator=(const KeycodeToTextConvertMappingStrategy &_other);
    KeycodeToTextConvertMappingStrategy &operator=(KeycodeToTextConvertMappingStrategy &&_other);


/// From KeycodeToTextConvertStrategyInterface
/// implemented
    virtual QString keyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool hasKeyText(const int _keycode, const int _layoutIndex, const bool _shift = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QString keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                   const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;

protected:
    void clear();
    void reset(const int _layoutCount);
    void addLayout();
    void removeTailLayout();
    void removeLayout(const int i);
    int getLayoutCount() const;
    void doSetKeyText(const int _keycode, const QString &_text, const int _layoutIndex, const bool _shift = false);
    void setLayoutsContainer(const LayoutsType &_layoutsContainer);
    void setShiftedLayoutsContainer(const LayoutsType &_shiftedLayoutsContainer);

private:
    struct ConvertMappingStrategyPrivate;
    std::unique_ptr<ConvertMappingStrategyPrivate> pConvertMappingStrategyPrivate;
};

} // qt_keyboard

#endif // KEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H
