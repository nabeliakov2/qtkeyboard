#ifndef SINGLELAYOUTKEYCODEONLYTOTEXTCONVERTMAPPINGSTRATEGY_H
#define SINGLELAYOUTKEYCODEONLYTOTEXTCONVERTMAPPINGSTRATEGY_H

/**
 * For keeping common values for all layout
 * _layoutIndex, _shift and _caps argumnets are ignored
 *
**/

#include "core/backend/keycodetotextconverting/mapping/keycodetotextconvertmappingstrategy.h"
#include "core/backend/keycodetotextconverting/utilities/keycodetotextconvertutility.h"

namespace qt_keyboard
{

class SingleLayoutKeycodeOnlyToTextConvertMappingStrategy : public KeycodeToTextConvertStrategyInterface
{
public:
    SingleLayoutKeycodeOnlyToTextConvertMappingStrategy() = default;
    virtual ~SingleLayoutKeycodeOnlyToTextConvertMappingStrategy() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE = default;

    SingleLayoutKeycodeOnlyToTextConvertMappingStrategy(const SingleLayoutKeycodeOnlyToTextConvertMappingStrategy &_other);
    SingleLayoutKeycodeOnlyToTextConvertMappingStrategy(SingleLayoutKeycodeOnlyToTextConvertMappingStrategy &&_other);

    SingleLayoutKeycodeOnlyToTextConvertMappingStrategy &operator=(const SingleLayoutKeycodeOnlyToTextConvertMappingStrategy &_other);
    SingleLayoutKeycodeOnlyToTextConvertMappingStrategy &operator=(SingleLayoutKeycodeOnlyToTextConvertMappingStrategy &&_other);

/// From KeycodeToTextConvertStrategyInterface
/// implemented
    virtual QString keyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool hasKeyText(const int _keycode, const int _layoutIndex, const bool _shift = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QString keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                   const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;

protected:
    inline void clear() {m_mapingContainer.clear();}
    inline void reset(const int) {m_mapingContainer.clear();}
    inline int getLayoutCount() const {return 1;}
    inline void doSetKeyText(const int _keycode, const QString &_text, const int = 0, const bool = false)
    {
        m_mapingContainer.insert(_keycode, _text);
    }
    void setLayoutsContainer(const MappingType &_layoutsContainer)
    {
        if (m_mapingContainer.size() != _layoutsContainer.size() ||
                m_mapingContainer != _layoutsContainer)
        {
            m_mapingContainer = _layoutsContainer;
        }
    }

private:
    MappingType m_mapingContainer;

};

} // qt_keyboard

#endif // SINGLELAYOUTKEYCODEONLYTOTEXTCONVERTMAPPINGSTRATEGY_H
