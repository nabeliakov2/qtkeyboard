#ifndef KEYBOARDLAYOUT_H
#define KEYBOARDLAYOUT_H

#include <QtCore/QList>
#include <QtCore/QSharedPointer>

#include <memory>

namespace qt_keyboard
{

struct KeyboardKeySetConfig Q_DECL_FINAL
{
    QString id;
    QList<QList<int>> keyCodeList;
};

class KeyboardKeySet Q_DECL_FINAL
{
public:
    explicit KeyboardKeySet(const QSharedPointer<const KeyboardKeySetConfig> &_pConfig);
    ~KeyboardKeySet() = default;

// deeply copyable and moveable
    KeyboardKeySet(const KeyboardKeySet &_other);
    KeyboardKeySet(KeyboardKeySet &&_other);
    KeyboardKeySet &operator=(const KeyboardKeySet &_other);
    KeyboardKeySet &operator=(KeyboardKeySet &&_other);


    bool isEmpty() const;
    int rowCount() const Q_DECL_NOEXCEPT;
    int keyCount(const int _row) const;
    QList<int> keyRow(const int _row) const;
    int keycodeAt(const int _row, const int _pos) const;
    QList<QList<int>> keySet() const;

private:
    QSharedPointer<const KeyboardKeySetConfig> pKeySetConfigPrivate;

    void checkRow(const int _row) const;
};

} // qt_keyboard

#endif // KEYBOARDLAYOUT_H
