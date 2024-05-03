#include "core/backend/keycodetotextconverting/mapping/keycodetotextconvertmappingstrategy.h"

namespace qt_keyboard
{

struct KeycodeToTextConvertMappingStrategy::ConvertMappingStrategyPrivate Q_DECL_FINAL // private
{
// sizes MUST be synchronized (and be equal to current layout count)
    LayoutsType layoutsContainer;
    LayoutsType shiftedsLayoutContainer;

    inline MappingType &getLayout(const int _layoutIndex, const bool _shift = false)
    {
        return _shift ? layoutsContainer[_layoutIndex] : shiftedsLayoutContainer[_layoutIndex];
    }
    inline const MappingType &getLayout(const int _layoutIndex, const bool _shift = false) const
    {
        return _shift ? layoutsContainer.at(_layoutIndex) : shiftedsLayoutContainer.at(_layoutIndex);
    }
    inline void checkLayoutIndex(const int _layoutIndex, const std::string &_where) const
    {
        if (_layoutIndex < 0 || _layoutIndex >= layoutsContainer.size())
        {
            throw std::invalid_argument(std::string("Invalid layout index (") + std::to_string(_layoutIndex) +
                                        std::string(") at ") + _where);
        }
    }
};

KeycodeToTextConvertMappingStrategy::KeycodeToTextConvertMappingStrategy()  // public
    : KeycodeToTextConvertStrategyInterface(),
      pConvertMappingStrategyPrivate(new ConvertMappingStrategyPrivate)
{}

KeycodeToTextConvertMappingStrategy::~KeycodeToTextConvertMappingStrategy() Q_DECL_NOEXCEPT // virtual public override final
{}

KeycodeToTextConvertMappingStrategy::KeycodeToTextConvertMappingStrategy(const KeycodeToTextConvertMappingStrategy &_other)
    : KeycodeToTextConvertStrategyInterface(_other),
      pConvertMappingStrategyPrivate(new ConvertMappingStrategyPrivate(*_other.pConvertMappingStrategyPrivate))
{}

KeycodeToTextConvertMappingStrategy::KeycodeToTextConvertMappingStrategy(KeycodeToTextConvertMappingStrategy &&_other)
    : KeycodeToTextConvertStrategyInterface(_other),
      pConvertMappingStrategyPrivate(std::move(_other.pConvertMappingStrategyPrivate))
{}

KeycodeToTextConvertMappingStrategy &KeycodeToTextConvertMappingStrategy::operator=(const KeycodeToTextConvertMappingStrategy &_other)
{
    if (&_other != this)
    {
        KeycodeToTextConvertStrategyInterface::operator=(_other);
        *pConvertMappingStrategyPrivate = *_other.pConvertMappingStrategyPrivate;
    }
    return *this;
}

KeycodeToTextConvertMappingStrategy &KeycodeToTextConvertMappingStrategy::operator=(KeycodeToTextConvertMappingStrategy &&_other)
{
    if (&_other != this)
    {
        KeycodeToTextConvertStrategyInterface::operator=(_other);
        pConvertMappingStrategyPrivate = std::move(_other.pConvertMappingStrategyPrivate);
    }
    return *this;
}

QString KeycodeToTextConvertMappingStrategy::keyText(const int _keycode, const int _layoutIndex,
                                                     const bool _shift /* = false */, const bool _caps /* = false */) const  // virtual public override final
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    if (pConvertMappingStrategyPrivate -> layoutsContainer.isEmpty()) return {};
    pConvertMappingStrategyPrivate -> checkLayoutIndex(_layoutIndex, Q_FUNC_INFO);
    const auto &l_layout(pConvertMappingStrategyPrivate -> getLayout(_layoutIndex, _shift));
    QString l_text(l_layout.value(_keycode));
    capsTransform(l_text, _shift, _caps);
    return l_text;
}

bool KeycodeToTextConvertMappingStrategy::hasKeyText(const int _keycode, const int _layoutIndex, const bool _shift) const   // virtual public override final
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    if (pConvertMappingStrategyPrivate -> layoutsContainer.isEmpty()) return false;
    pConvertMappingStrategyPrivate -> checkLayoutIndex(_layoutIndex, Q_FUNC_INFO);
    const auto &l_layout(pConvertMappingStrategyPrivate -> getLayout(_layoutIndex, _shift));
    return l_layout.contains(_keycode);
}

QString KeycodeToTextConvertMappingStrategy::keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                                            const bool _shift, const bool _caps) const // virtual public override final
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    if (pConvertMappingStrategyPrivate -> layoutsContainer.isEmpty())
    {
        _hasKeyTextReturn = false;
        return {};
    }
    pConvertMappingStrategyPrivate -> checkLayoutIndex(_layoutIndex, Q_FUNC_INFO);
    const auto &l_layout(pConvertMappingStrategyPrivate -> getLayout(_layoutIndex, _shift));
    _hasKeyTextReturn = l_layout.contains(_keycode);
    if (_hasKeyTextReturn)
    {
        QString l_text(l_layout.value(_keycode));
        capsTransform(l_text, _shift, _caps);
        return l_text;
    }
    return {};
}

void KeycodeToTextConvertMappingStrategy::clear()   // protected
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    pConvertMappingStrategyPrivate -> layoutsContainer.clear();
    pConvertMappingStrategyPrivate -> shiftedsLayoutContainer.clear();
}

void KeycodeToTextConvertMappingStrategy::reset(const int _layoutCount) // public
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    if (_layoutCount < 0)
    {
        throw std::invalid_argument(std::string("Layout count must be >= 0 in ") + Q_FUNC_INFO);
    }

    clear();
    if (_layoutCount > 0)
    {
        pConvertMappingStrategyPrivate -> layoutsContainer.resize(_layoutCount);
        pConvertMappingStrategyPrivate -> layoutsContainer.reserve(_layoutCount);
        pConvertMappingStrategyPrivate -> shiftedsLayoutContainer.resize(_layoutCount);
        pConvertMappingStrategyPrivate -> shiftedsLayoutContainer.reserve(_layoutCount);
    }

}

void KeycodeToTextConvertMappingStrategy::addLayout() // protected
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    pConvertMappingStrategyPrivate -> layoutsContainer.push_back({});
    pConvertMappingStrategyPrivate -> shiftedsLayoutContainer.push_back({});
}

void KeycodeToTextConvertMappingStrategy::removeTailLayout()    // protected
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    pConvertMappingStrategyPrivate -> layoutsContainer.removeLast();
    pConvertMappingStrategyPrivate -> shiftedsLayoutContainer.removeLast();
}

void KeycodeToTextConvertMappingStrategy::removeLayout(const int i) // protected
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    pConvertMappingStrategyPrivate -> checkLayoutIndex(i, Q_FUNC_INFO);

    pConvertMappingStrategyPrivate -> layoutsContainer.removeAt(i);
    pConvertMappingStrategyPrivate -> shiftedsLayoutContainer.removeAt(i);
}

int KeycodeToTextConvertMappingStrategy::getLayoutCount() const // protected
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    return pConvertMappingStrategyPrivate -> layoutsContainer.count();
}

void KeycodeToTextConvertMappingStrategy::doSetKeyText(const int _keycode, const QString &_text, const int _layoutIndex, const bool _shift) // protected
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    pConvertMappingStrategyPrivate -> checkLayoutIndex(_layoutIndex, Q_FUNC_INFO);
    auto &l_layout(pConvertMappingStrategyPrivate -> getLayout(_layoutIndex, _shift));
    l_layout.insert(_keycode, _text);
}

void KeycodeToTextConvertMappingStrategy::setLayoutsContainer(const LayoutsType &_layoutsContainer)  // protected
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    if (pConvertMappingStrategyPrivate -> layoutsContainer != _layoutsContainer)
    {
        pConvertMappingStrategyPrivate -> layoutsContainer = _layoutsContainer;
    }
}

void KeycodeToTextConvertMappingStrategy::setShiftedLayoutsContainer(const LayoutsType &_shiftedLayoutsContainer)
{
    Q_ASSERT(pConvertMappingStrategyPrivate);

    if (pConvertMappingStrategyPrivate -> shiftedsLayoutContainer != _shiftedLayoutsContainer)
    {
        pConvertMappingStrategyPrivate -> shiftedsLayoutContainer = _shiftedLayoutsContainer;
    }
}


} // qt_keyboard
