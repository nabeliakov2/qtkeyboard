#ifndef KEYSETUPDATEDKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H
#define KEYSETUPDATEDKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H

#include "core/backend/keycodetotextconverting/mapping/keycodetotextconvertmappingstrategy.h"

namespace qt_keyboard
{

class KeyboardKeySet;
class AbstractKeyboardBackend;

class KeySetUpdatedKeycodeToTextConvertMappingStrategy Q_DECL_FINAL : public KeycodeToTextConvertMappingStrategy
{
public:
    explicit KeySetUpdatedKeycodeToTextConvertMappingStrategy() = default;
    virtual ~KeySetUpdatedKeycodeToTextConvertMappingStrategy() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL = default;

/// All pointers are weak, no thread sync is performed (only entrance nullptr control)
    void updateLayouts(const KeyboardKeySet *const _pKeySet,
                       const QStringList &_layoutList,
                       const KeycodeToTextConvertStrategyInterface *const _pKeyTextSrc);
    void clearLayouts() {clear();}
};

} // qt_keyboard

#endif // KEYSETUPDATEDKEYCODETOTEXTCONVERTMAPPINGSTRATEGY_H
