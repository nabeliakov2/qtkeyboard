#include "core/backend/keysets/keyboardkeyset.h"

namespace qt_keyboard
{

KeyboardKeySet::KeyboardKeySet(const QSharedPointer<const KeyboardKeySetConfig> &_pConfig)    // public
    : pKeySetConfigPrivate(_pConfig)
{
    if (!_pConfig)
    {
        throw std::invalid_argument(std::string("Keyboard KeySet config cannot be empty in ") + Q_FUNC_INFO);
    }
}

KeyboardKeySet::KeyboardKeySet(const KeyboardKeySet &_other)    // public
    : pKeySetConfigPrivate(new KeyboardKeySetConfig(*_other.pKeySetConfigPrivate))
{
    Q_ASSERT(_other.pKeySetConfigPrivate);
}

KeyboardKeySet::KeyboardKeySet(KeyboardKeySet &&_other)    // public
    : pKeySetConfigPrivate(std::move(_other.pKeySetConfigPrivate))
{}

KeyboardKeySet &KeyboardKeySet::operator=(const KeyboardKeySet &_other) // public
{
    Q_ASSERT(pKeySetConfigPrivate && _other.pKeySetConfigPrivate);

    if (this != &_other)
    {
        pKeySetConfigPrivate.reset();
        pKeySetConfigPrivate.reset(new KeyboardKeySetConfig(*_other.pKeySetConfigPrivate));
    }
    return *this;
}

KeyboardKeySet &KeyboardKeySet::operator=(KeyboardKeySet &&_other)  // public
{
    Q_ASSERT(pKeySetConfigPrivate && _other.pKeySetConfigPrivate);

    if (this != &_other)
    {
        pKeySetConfigPrivate = std::move(_other.pKeySetConfigPrivate);
    }
    return *this;
}

bool KeyboardKeySet::isEmpty() const    // public
{
    Q_ASSERT(pKeySetConfigPrivate);

    return pKeySetConfigPrivate -> keyCodeList.isEmpty();
}

int KeyboardKeySet::rowCount() const Q_DECL_NOEXCEPT    // public
{
    Q_ASSERT(pKeySetConfigPrivate);

    return pKeySetConfigPrivate -> keyCodeList.count();
}

int KeyboardKeySet::keyCount(const int _row) const  // public
{
    checkRow(_row);
    return pKeySetConfigPrivate -> keyCodeList.at(_row).size();
}

QList<int> KeyboardKeySet::keyRow(const int _row) const // public
{
    checkRow(_row);
    return pKeySetConfigPrivate -> keyCodeList.at(_row);
}

int KeyboardKeySet::keycodeAt(const int _row, const int _pos) const  // public
{
    Q_ASSERT(pKeySetConfigPrivate);

    checkRow(_row);
    const QList<int> &l_row(pKeySetConfigPrivate -> keyCodeList.at(_row));

    if (_pos < 0 || _pos >= l_row.size())
    {
        throw std::invalid_argument(std::string("Invalid key (") + std::to_string(_pos) + ") in " + Q_FUNC_INFO);
    }

    return l_row.at(_pos);
}

QList<QList<int>> KeyboardKeySet::keySet() const    // public
{
    Q_ASSERT(pKeySetConfigPrivate);

    return pKeySetConfigPrivate -> keyCodeList;
}

void KeyboardKeySet::checkRow(const int _row) const   // private
{
    Q_ASSERT(pKeySetConfigPrivate);

    if (_row < 0 || _row >= pKeySetConfigPrivate -> keyCodeList.size())
    {
        throw std::invalid_argument(std::string("Invalid row (") + std::to_string(_row) + ") in " + Q_FUNC_INFO);
    }
}







} // qt_keyboard
