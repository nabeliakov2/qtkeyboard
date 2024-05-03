#ifndef MODIFIABLEKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H
#define MODIFIABLEKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H

#include "core/backend/keycodetotextconverting/mapping/keycodetotextconvertmappingstrategy.h"

namespace qt_keyboard
{

template <typename MappingConverterType>
class ModifiableKeycodeToTextConvertMappingStrategyTemplate Q_DECL_FINAL : public MappingConverterType
{
public:
    ModifiableKeycodeToTextConvertMappingStrategyTemplate() = default;
    virtual ~ModifiableKeycodeToTextConvertMappingStrategyTemplate() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL = default;

    ModifiableKeycodeToTextConvertMappingStrategyTemplate(const ModifiableKeycodeToTextConvertMappingStrategyTemplate &_other) = default;
    ModifiableKeycodeToTextConvertMappingStrategyTemplate(ModifiableKeycodeToTextConvertMappingStrategyTemplate &&_other) = default;

    ModifiableKeycodeToTextConvertMappingStrategyTemplate &operator=(const ModifiableKeycodeToTextConvertMappingStrategyTemplate &_other) = default;
    ModifiableKeycodeToTextConvertMappingStrategyTemplate &operator=(ModifiableKeycodeToTextConvertMappingStrategyTemplate &&_other) = default;

    inline void clearAll() {MappingConverterType::clear();}
    inline void resetLayouts(const int _count) {MappingConverterType::reset(_count);}
    inline int layoutCount() const {return MappingConverterType::getLayoutCount();}
    inline void setKeyText(const int _keycode, const QString &_text, const int _layoutIndex, const bool _shift = false)
    {
        MappingConverterType::doSetKeyText(_keycode, _text, _layoutIndex, _shift);
    }
};




} // qt_keyboard

#endif // MODIFIABLEKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H
