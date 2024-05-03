#ifndef DIRECTSETKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H
#define DIRECTSETKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H

#include "core/backend/keycodetotextconverting/mapping/keycodetotextconvertmappingstrategy.h"

namespace qt_keyboard
{

class DirectSetKeycodeToTextConvertMappingStrategy Q_DECL_FINAL : public KeycodeToTextConvertMappingStrategy
{
public:
    DirectSetKeycodeToTextConvertMappingStrategy(const LayoutsType &_layoutList,
                                                 const LayoutsType &_shiftedLayoutList);
    explicit DirectSetKeycodeToTextConvertMappingStrategy(const LayoutsType &_layoutList, const bool _shift = false);
    virtual ~DirectSetKeycodeToTextConvertMappingStrategy() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL = default;

    DirectSetKeycodeToTextConvertMappingStrategy(const DirectSetKeycodeToTextConvertMappingStrategy &_other) = default;
    DirectSetKeycodeToTextConvertMappingStrategy(DirectSetKeycodeToTextConvertMappingStrategy &&_other) = default;

    DirectSetKeycodeToTextConvertMappingStrategy &operator=(const DirectSetKeycodeToTextConvertMappingStrategy &_other) = default;
    DirectSetKeycodeToTextConvertMappingStrategy &operator=(DirectSetKeycodeToTextConvertMappingStrategy &&_other) = default;


};

} // qt_keyboard

#endif // DIRECTSETKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H
