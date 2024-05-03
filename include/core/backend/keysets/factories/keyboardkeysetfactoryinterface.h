#ifndef KEYBOARDKEYSETFACTORYINTERFACE_H
#define KEYBOARDKEYSETFACTORYINTERFACE_H

namespace qt_keyboard
{

struct KeyboardKeySet;

class KeyboardKeySetFactoryInterface
{
public:
    KeyboardKeySetFactoryInterface() = default;
    virtual ~KeyboardKeySetFactoryInterface() = default;

    KeyboardKeySetFactoryInterface(const KeyboardKeySetFactoryInterface &_other) = default;
    KeyboardKeySetFactoryInterface(KeyboardKeySetFactoryInterface &&_other) = default;

    KeyboardKeySetFactoryInterface &operator=(const KeyboardKeySetFactoryInterface &_other) = default;
    KeyboardKeySetFactoryInterface &operator=(KeyboardKeySetFactoryInterface &&_other) = default;


    virtual KeyboardKeySet *createKeyboardKeySet() const = 0;
};

} // qt_keyboard

#endif // KEYBOARDKEYSETFACTORYINTERFACE_H
