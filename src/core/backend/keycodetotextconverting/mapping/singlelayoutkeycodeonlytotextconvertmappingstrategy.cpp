#include "core/backend/keycodetotextconverting/mapping/singlelayoutkeycodeonlytotextconvertmappingstrategy.h"

namespace qt_keyboard
{

QString SingleLayoutKeycodeOnlyToTextConvertMappingStrategy::keyText(const int _keycode,
                                                                     const int, const bool, const bool) const  // virtual public override final
{
    if (m_mapingContainer.isEmpty()) return {};
    return m_mapingContainer.value(_keycode);
}

bool SingleLayoutKeycodeOnlyToTextConvertMappingStrategy::hasKeyText(const int _keycode,
                                                                     const int, const bool) const   // virtual public override final
{
    if (m_mapingContainer.isEmpty()) return false;
    return m_mapingContainer.contains(_keycode);
}

QString SingleLayoutKeycodeOnlyToTextConvertMappingStrategy::keyTextChecked(bool &_hasKeyTextReturn, const int _keycode,
                                                                            const int, const bool, const bool) const // virtual public override final
{
    if (m_mapingContainer.isEmpty())
    {
        _hasKeyTextReturn = false;
        return {};
    }
    _hasKeyTextReturn = m_mapingContainer.contains(_keycode);
    if (_hasKeyTextReturn)
    {
        return m_mapingContainer.value(_keycode);
    }
    return {};
}

} // qt_keyboard
