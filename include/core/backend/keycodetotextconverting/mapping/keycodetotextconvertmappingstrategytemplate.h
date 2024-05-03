#ifndef KEYCODETOTEXTCONVERTMAPPINGSTRATEGYTEMPLATE_H
#define KEYCODETOTEXTCONVERTMAPPINGSTRATEGYTEMPLATE_H

/**
 * ListType must support:
 * MappingType as an element,
 * Ctors: default, copy, move,
 * Dtor,
 * size() -> int
 * at(int) -> const MappingType&
 *
 * MappingType must support:
 * Ctors: default, copy, move,
 * Dtor,
 * value(int) -> QString
 *
 * both containers must have the same size
 * (layout count)
**/

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QHash>

#include "keycodetotextconvertstrategyinterface.h"

#include <string>

namespace qt_keyboard
{
template <typename ListType, typename MappingType>
class KeycodeToTextConvertMappingStrategyTemplate : public KeycodeToTextConvertStrategyInterface
{
public:
    KeycodeToTextConvertMappingStrategyTemplate() = default;
    virtual ~KeycodeToTextConvertMappingStrategyTemplate() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE = default;

    KeycodeToTextConvertMappingStrategyTemplate(const KeycodeToTextConvertMappingStrategyTemplate &_other) = default;
    KeycodeToTextConvertMappingStrategyTemplate(KeycodeToTextConvertMappingStrategyTemplate &&_other) = default;

    KeycodeToTextConvertMappingStrategyTemplate &operator=(const KeycodeToTextConvertMappingStrategyTemplate &_other) = default;
    KeycodeToTextConvertMappingStrategyTemplate &operator=(KeycodeToTextConvertMappingStrategyTemplate &&_other) = default;

    virtual bool hasKeyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false) const Q_DECL_OVERRIDE
    {
        if (m_layoutContainer.isEmpty()) return false;
        checkLayoutIndex(_layoutIndex, Q_FUNC_INFO);
        const MappingType &l_layout(getLayout(_layoutIndex, _shift));
        return l_layout.contains(_keycode);
    }
    virtual QString keyText(const int _keycode, const int _layoutIndex,
                         const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE
    {
        if (m_layoutContainer.isEmpty()) return {};
        checkLayoutIndex(_layoutIndex, Q_FUNC_INFO);
        const MappingType &l_layout(getLayout(_layoutIndex, _shift));
        QString l_text(l_layout.value(_keycode));
        capsTransform(l_text, _shift, _caps);
        return l_text;
    }
    virtual QString keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                   const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE
    {
        if (m_layoutContainer.isEmpty())
        {
            _hasKeyTextReturn = false;
            return {};
        }
        checkLayoutIndex(_layoutIndex, Q_FUNC_INFO);
        const MappingType &l_layout(getLayout(_layoutIndex, _shift));
        _hasKeyTextReturn = l_layout.contains(_keycode);
        if (_hasKeyTextReturn)
        {
            QString l_text(l_layout.value(_keycode));
            capsTransform(l_text, _shift, _caps);
            return l_text;
        }
        return {};
    }


private:
    ListType m_layoutContainer;
    ListType m_shiftedLayoutContainer;

    inline const MappingType &getLayout(const int _layoutIndex, const bool _shift = false) const
    {
        return _shift ? m_layoutContainer.at(_layoutIndex) : m_shiftedLayoutContainer.at(_layoutIndex);
    }
    inline void checkLayoutIndex(const int _layoutIndex, const std::string &_where) const
    {
        if (_layoutIndex < 0 || _layoutIndex >= m_layoutContainer.size())
        {
            throw std::invalid_argument(std::string("Invalid layout index (") + std::to_string(_layoutIndex) +
                                        std::string(") at ") + _where);
        }
    }
};

// A common used specialization
using HasLayouthMappingType = QHash<int, QString>;
using ListHashKeycodeToTextConvertMappingStrategy = KeycodeToTextConvertMappingStrategyTemplate<QList<HasLayouthMappingType>, HasLayouthMappingType>;

} // qt_keyboard




#endif // KEYCODETOTEXTCONVERTMAPPINGSTRATEGYTEMPLATE_H
