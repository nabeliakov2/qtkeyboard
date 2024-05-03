#include "core/backend/keycodetotextconverting/mapping/directsetkeycodetotextconvertmappingstrategy.h"

namespace qt_keyboard
{

DirectSetKeycodeToTextConvertMappingStrategy::DirectSetKeycodeToTextConvertMappingStrategy(const LayoutsType &_layoutList,
                                                                                           const LayoutsType &_shiftedLayoutList)    // puilic
    : KeycodeToTextConvertMappingStrategy()
{
    setLayoutsContainer(_layoutList);
    setShiftedLayoutsContainer(_shiftedLayoutList);
}

DirectSetKeycodeToTextConvertMappingStrategy::DirectSetKeycodeToTextConvertMappingStrategy(const LayoutsType &_layoutList,
                                                                                           const bool _shift /* = false */) // explicit public
    : KeycodeToTextConvertMappingStrategy()
{
    if (_shift)
    {
        setShiftedLayoutsContainer(_layoutList);
    }
    else
    {
        setLayoutsContainer(_layoutList);
    }
}



} // qt_keyboard
