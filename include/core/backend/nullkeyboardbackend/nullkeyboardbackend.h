#ifndef NULLKEYBOARDBACKEND_H
#define NULLKEYBOARDBACKEND_H

#include <QtCore/QObject>
#include <QtCore/QSet>

#include "core/backend/base/abstractkeyboardbackend.h"

namespace qt_keyboard
{

class NullKeyboardBackend : public AbstractKeyboardBackend
{
    Q_OBJECT
    Q_DISABLE_COPY(NullKeyboardBackend)

public:
    explicit NullKeyboardBackend(QObject *_parent = Q_NULLPTR)
        : AbstractKeyboardBackend(_parent) {}
    virtual ~NullKeyboardBackend() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE = default;


    virtual QStringList layouts() const {return {};}
    virtual QString layout(const int) const {return {};}
    virtual int layoutCount() const {return 0;}
    virtual QSet<int> pressedKeys() const {return {};}
    virtual bool isKeyPressed(const int) const {return false;}
    virtual int currentLayoutIndex() const {return 0;}
    virtual KeyboardUtility::LocksStateFlags locksState() const {return KeyboardUtility::NoLocksFlag;}
    virtual bool isCapsLockOn() const {return false;}
    virtual bool isNumLockOn() const {return false;}
    virtual bool isScrollLockOn() const {return false;}

    virtual void findAloneWindow() const {}
    virtual void configureMainWidget(QWidget *const) const {}

    virtual void simulateKeyEvent(const int, const bool) const {}
    virtual QString keyText(const int) const {return {};}

    virtual bool isInitialized() const {return false;}

};

} // qt_keyboard

#endif // NULLKEYBOARDBACKEND_H
