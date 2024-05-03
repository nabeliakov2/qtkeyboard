#include "core/backend/base/abstractkeyboardbackend.h"

#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QtCore/QDebug>

#include "core/backend/keycodetotextconverting/mapping/modifiablekeycodetotextconvertmappingstrategy.h"

namespace qt_keyboard
{

using ModifiableKeycodeToTextConvertMappingStrategy =
        ModifiableKeycodeToTextConvertMappingStrategyTemplate<KeycodeToTextConvertMappingStrategy>;

struct AbstractKeyboardBackend::BackendPrivate  Q_DECL_FINAL // private
{
    QScopedPointer<ModifiableKeycodeToTextConvertMappingStrategy> pKeyTextCache;
    QSharedPointer<const KeycodeToTextConvertStrategyInterface> pForcedKeyText;
    QSharedPointer<const LayoutNameMapper> pLayoutNameMapper;

    QStringList layouts;
    int currentLayoutIndex {0};
    QSet<int> pressedKeys;
    KeyboardUtility::LocksStateFlags locksState {0x0};
    bool b_isInitialized {false};

    BackendPrivate()
        : pKeyTextCache(new ModifiableKeycodeToTextConvertMappingStrategy) {} // cache is on by default1
    ~BackendPrivate() Q_DECL_NOEXCEPT = default;

    void mapLayoutNames();

private:
    BackendPrivate(const BackendPrivate &_other) = delete;
    BackendPrivate(BackendPrivate &&_other) = delete;

    BackendPrivate &operator=(const BackendPrivate &_other) = delete;
    BackendPrivate &operator=(BackendPrivate &&_other) = delete;
};


AbstractKeyboardBackend::AbstractKeyboardBackend(QObject *_parent /* = Q_NULLPTR */)   // explicit public
    : QObject(_parent),
      pBackendPrivate(new BackendPrivate)
{}

AbstractKeyboardBackend::~AbstractKeyboardBackend() Q_DECL_NOEXCEPT // virtual public override
{}


void AbstractKeyboardBackend::setTextCached(const int _set) // public
{
    Q_ASSERT(pBackendPrivate);

    if (_set && !pBackendPrivate -> pKeyTextCache)
    {
        pBackendPrivate -> pKeyTextCache.reset(new ModifiableKeycodeToTextConvertMappingStrategy);
        if (pBackendPrivate -> b_isInitialized)
        {
            if (!pBackendPrivate -> layouts.isEmpty())
            {
                pBackendPrivate -> pKeyTextCache -> resetLayouts(pBackendPrivate -> layouts.count());
            }
        }
    }
    else if (!_set && pBackendPrivate -> pKeyTextCache)
    {
        pBackendPrivate -> pKeyTextCache.reset(Q_NULLPTR);
    }
}

bool AbstractKeyboardBackend::isTextCached() const  // publoc
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> pKeyTextCache;
}

void AbstractKeyboardBackend::setForcedKeycodeToText(const QSharedPointer<const KeycodeToTextConvertStrategyInterface> &_pForcedKeyText)    // public
{
    Q_ASSERT(pBackendPrivate);

    if (_pForcedKeyText && _pForcedKeyText.data() != pBackendPrivate -> pForcedKeyText.data())
    {
        pBackendPrivate -> pForcedKeyText = _pForcedKeyText;

        emit currentLayoutUpdated();
    }
}

const KeycodeToTextConvertStrategyInterface *AbstractKeyboardBackend::forcedKeycodeToText() const   // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> pForcedKeyText.data();
}

void AbstractKeyboardBackend::unsetForcedKeycodeToText()    // public
{
    Q_ASSERT(pBackendPrivate);

    pBackendPrivate -> pForcedKeyText.clear();
}

void AbstractKeyboardBackend::setLayoutNameMapper(const QSharedPointer<const LayoutNameMapper> &_pLayoutNameMapper)    // public
{
    Q_ASSERT(pBackendPrivate);

    if (_pLayoutNameMapper.data() != pBackendPrivate -> pLayoutNameMapper.data())
    {
        if (_pLayoutNameMapper)
        {
            if (!pBackendPrivate -> pLayoutNameMapper ||
                    *_pLayoutNameMapper != *pBackendPrivate -> pLayoutNameMapper)
            {
                pBackendPrivate -> pLayoutNameMapper = _pLayoutNameMapper;

                if (pBackendPrivate -> b_isInitialized)
                {
                    pBackendPrivate -> mapLayoutNames();
                } // -> if (pBackendPrivate -> b_isInitialized)
            } // -> if (!pBackendPrivate -> pLayoutNameMapper || *_pLayoutNameMapper != *pBackendPrivate -> pLayoutNameMapper)
        } // -> if (_pLayoutNameMapper)
        else
        {
            if (pBackendPrivate -> pLayoutNameMapper)
            {
                pBackendPrivate -> pLayoutNameMapper.clear();
            }
        }
    } // -> if (_pLayoutNameMapper.data() != pBackendPrivate -> pLayoutNameMapper.data())
}

const LayoutNameMapper *AbstractKeyboardBackend::layoutNameMapper() const   // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> pLayoutNameMapper.data();
}

void AbstractKeyboardBackend::unsetLayoutNameMapper()   // public
{
    Q_ASSERT(pBackendPrivate);

    pBackendPrivate -> pLayoutNameMapper.clear();
}

QString AbstractKeyboardBackend::text(const int _keycode) const // public
{
    Q_ASSERT(pBackendPrivate);

    const bool shiftState = pBackendPrivate -> pressedKeys.contains(KeyboardUtility::LeftShift) ||
            pBackendPrivate -> pressedKeys.contains(KeyboardUtility::RightShift);
    const bool capsState = pBackendPrivate -> locksState & KeyboardUtility::CapsLockFlag;

    if (pBackendPrivate -> pForcedKeyText)
    {
        if (pBackendPrivate -> pForcedKeyText -> hasKeyText(_keycode, pBackendPrivate -> currentLayoutIndex, shiftState))
        {
            return pBackendPrivate -> pForcedKeyText -> keyText(_keycode, pBackendPrivate -> currentLayoutIndex, shiftState, capsState);
        }
    }
    if (isTextCached())
    {
        Q_ASSERT(pBackendPrivate -> pKeyTextCache);

        if (pBackendPrivate -> pKeyTextCache -> hasKeyText(_keycode, pBackendPrivate -> currentLayoutIndex, shiftState))
        {
            return pBackendPrivate -> pKeyTextCache -> keyText(_keycode, pBackendPrivate -> currentLayoutIndex, shiftState, capsState);
        }
        bool s = false;
        QString l_text(keyTextChecked(s, _keycode, pBackendPrivate -> currentLayoutIndex, shiftState));

        if (s)
        {
            pBackendPrivate -> pKeyTextCache -> setKeyText(_keycode, l_text, pBackendPrivate -> currentLayoutIndex, shiftState);
            capsTransform(l_text, shiftState, capsState);
            return l_text;
        }
        return {};
    }
    bool s = false;
    QString l_text(keyTextChecked(s, _keycode, pBackendPrivate -> currentLayoutIndex, shiftState, capsState));
    return s ? l_text : QString();
}

bool AbstractKeyboardBackend::isInitialized() const Q_DECL_NOEXCEPT // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> b_isInitialized;
}

QStringList AbstractKeyboardBackend::layouts() const  // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> layouts;
}

QString AbstractKeyboardBackend::layout(const int i) const // public
{
    Q_ASSERT(pBackendPrivate);

    if (i < 0 || i >= pBackendPrivate -> layouts.size())
    {
        throw std::invalid_argument(std::string("Invalid layout index (") + std::to_string(i) + std::string(")") + Q_FUNC_INFO);
    }
    return pBackendPrivate -> layouts.at(i);
}

QString AbstractKeyboardBackend::currentLayout() const  // public
{
    Q_ASSERT(pBackendPrivate);

    if (!pBackendPrivate -> layouts.isEmpty())
    {
        if (pBackendPrivate -> currentLayoutIndex > -1 &&
                pBackendPrivate -> currentLayoutIndex < pBackendPrivate -> layouts.size())
        {
            return pBackendPrivate -> layouts.at(pBackendPrivate -> currentLayoutIndex);
        }
        else
        {
            qWarning() << "Something wrong in" << Q_FUNC_INFO;
        }
    }
    return {};
}

int AbstractKeyboardBackend::layoutCount() const  // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> layouts.count();
}

QSet<int> AbstractKeyboardBackend::pressedKeys() const    // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> pressedKeys;
}

bool AbstractKeyboardBackend::isKeyPressed(const int _keycode) const  // public
{
    if (_keycode < 0)
    {
        throw std::invalid_argument(std::string("Invalid keykode (") + std::to_string(_keycode) + std::string(") in ") + Q_FUNC_INFO);
    }

    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> pressedKeys.contains(_keycode);
}

int AbstractKeyboardBackend::currentLayoutIndex() const Q_DECL_NOEXCEPT   // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> currentLayoutIndex;
}

KeyboardUtility::LocksStateFlags AbstractKeyboardBackend::locksState() const Q_DECL_NOEXCEPT // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> locksState;
}

bool AbstractKeyboardBackend::isCapsLockOn() const Q_DECL_NOEXCEPT // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> locksState & KeyboardUtility::CapsLockFlag;
}

bool AbstractKeyboardBackend::isNumLockOn() const Q_DECL_NOEXCEPT // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> locksState & KeyboardUtility::NumLockFlag;
}

bool AbstractKeyboardBackend::isScrollLockOn() const Q_DECL_NOEXCEPT // public
{
    Q_ASSERT(pBackendPrivate);

    return pBackendPrivate -> locksState & KeyboardUtility::ScrollLockFlag;
}

void AbstractKeyboardBackend::switchToNextLayout() // public
{
    Q_ASSERT(pBackendPrivate);

    if (!pBackendPrivate -> layouts.isEmpty())
    {
        const int nextLayoutIndex = (pBackendPrivate -> currentLayoutIndex + 1) % pBackendPrivate -> layouts.size();
        doSwitchToLayout(nextLayoutIndex);
    }
}

void AbstractKeyboardBackend::switchToLayout(const int _layoutIndex)    // public
{
    Q_ASSERT(pBackendPrivate);

    if (!pBackendPrivate -> layouts.isEmpty())
    {
        if (_layoutIndex < 0 || _layoutIndex >= pBackendPrivate -> layouts.size())
        {
            throw std::invalid_argument(std::string("Invalid layout index (") + std::to_string(_layoutIndex)
                                        + std::string(")") + Q_FUNC_INFO);
        }

        doSwitchToLayout(_layoutIndex);
    }
}

void AbstractKeyboardBackend::updateInitialized()   // protected
{
    Q_ASSERT(pBackendPrivate);

    if (!pBackendPrivate -> b_isInitialized)
    {
         pBackendPrivate -> b_isInitialized = true;
         emit initialized();
    }
    else
    {
        qWarning("Init warning");
    }
}

void AbstractKeyboardBackend::updateLayouts(const QStringList &_newLayouts) // protected
{
    Q_ASSERT(pBackendPrivate);

    if (_newLayouts.size() != pBackendPrivate -> layouts.size() ||
            _newLayouts != pBackendPrivate -> layouts)
    {
        pBackendPrivate -> layouts = _newLayouts;

        if (isTextCached())
        {
            Q_ASSERT(pBackendPrivate -> pKeyTextCache);

            if (_newLayouts.isEmpty())
            {
                pBackendPrivate -> pKeyTextCache -> clearAll();
            }
            else
            {
                pBackendPrivate -> pKeyTextCache -> resetLayouts(_newLayouts.count());
                if (pBackendPrivate -> pLayoutNameMapper)
                {
                    pBackendPrivate -> mapLayoutNames();
                }
            }
        }
        emit layoutsChanged(pBackendPrivate -> layouts);
    }
}

void AbstractKeyboardBackend::updateCurrentLayout(const int _newCurrentLayout)  // protected
{
    Q_ASSERT(pBackendPrivate);

    if (_newCurrentLayout != pBackendPrivate -> currentLayoutIndex)
    {
        pBackendPrivate -> currentLayoutIndex = _newCurrentLayout;

        emit currentLayoutChanged(pBackendPrivate -> currentLayoutIndex);
    }
}

void AbstractKeyboardBackend::updateLockState(const KeyboardUtility::LocksStateFlags &_newLockState)    // protected
{
    Q_ASSERT(pBackendPrivate);

    if (_newLockState != pBackendPrivate -> locksState)
    {
        const KeyboardUtility::LocksStateFlags prevLockState = pBackendPrivate -> locksState;
        pBackendPrivate -> locksState = _newLockState;

        emit lockStateChanged(pBackendPrivate -> locksState, prevLockState);
    }
}

void AbstractKeyboardBackend::updateKeyState(const int _keycode, const bool _pressed)    // protected
{
    Q_ASSERT(pBackendPrivate);

    if (_pressed)
    {
        if (!pBackendPrivate -> pressedKeys.contains(_keycode))
        {
            pBackendPrivate -> pressedKeys.insert(_keycode);
            emit keysUpdated(_keycode, _pressed);
        }
    }
    else
    {
        if (pBackendPrivate -> pressedKeys.contains(_keycode))
        {
            pBackendPrivate -> pressedKeys.remove(_keycode);
            emit keysUpdated(_keycode, _pressed);
        }
    }
}

//////////////////////////////////////////////////////////////////
/// BackendPrivate
void AbstractKeyboardBackend::BackendPrivate::mapLayoutNames()  // private::public
{
    Q_ASSERT(pLayoutNameMapper);

    for (QString &currLayout : layouts)
    {
        if (pLayoutNameMapper -> contains(currLayout))
        {
            currLayout = pLayoutNameMapper -> value(currLayout);
        }
    }
}

} // qt_keyboard
