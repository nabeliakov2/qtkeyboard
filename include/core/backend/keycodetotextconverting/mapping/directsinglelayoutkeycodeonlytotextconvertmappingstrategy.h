#ifndef DIRECTSINGLELAYOUTKEYCODEONLYTOTEXTCONVERTMAPPINGSTRATEGY_H
#define DIRECTSINGLELAYOUTKEYCODEONLYTOTEXTCONVERTMAPPINGSTRATEGY_H

#include "core/backend/keycodetotextconverting/mapping/singlelayoutkeycodeonlytotextconvertmappingstrategy.h"

namespace qt_keyboard
{

class DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy Q_DECL_FINAL : public SingleLayoutKeycodeOnlyToTextConvertMappingStrategy
{
public:
    explicit DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy(const MappingType &_layoutsContainer);
    virtual ~DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL = default;

    DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy(const DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy &_other) = default;
    DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy(DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy &&_other) = default;

    DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy &operator=(const DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy &_other) = default;
    DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy &operator=(DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy &&_other) = default;
};

} // qt_keyboard

#endif // DIRECTSINGLELAYOUTKEYCODEONLYTOTEXTCONVERTMAPPINGSTRATEGY_H
