#ifndef X11KEYBOARDBACKEND_H
#define X11KEYBOARDBACKEND_H

#include <QtGlobal>

#ifdef Q_OS_LINUX

#include <memory>

#include "core/backend/base/abstractkeyboardbackend.h"

namespace qt_keyboard
{

class X11KeyboardBackend : public AbstractKeyboardBackend
{
    Q_OBJECT
    Q_DISABLE_COPY(X11KeyboardBackend)

public:
    explicit X11KeyboardBackend(QObject *_parent = Q_NULLPTR);
    virtual ~X11KeyboardBackend() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE;

/// From AbstractKeyboardBackend
/// implemented
    virtual void findAloneWindow() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void configureMainWidget(QWidget *const _pWidget) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void simulateKeyEvent(const int _keycode, const bool _pressed) const Q_DECL_OVERRIDE Q_DECL_FINAL;

/// From KeycodeToTextConvertStrategyInterface
/// implemented
    virtual QString keyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QString keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                   const bool _shift = false, const bool _caps = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool hasKeyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false) const Q_DECL_OVERRIDE Q_DECL_FINAL;


protected:
    virtual void doSwitchToLayout(const int _layoutIndex) Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    struct X11KeyboardBackendPrivate;
    const std::unique_ptr<X11KeyboardBackendPrivate> pX11Private;

    void init();
    void handleSigTerm(const int _socket) Q_DECL_NOEXCEPT;
};

} // qt_keyboard

#endif  // Q_OS_LINUX

#endif // X11KEYBOARDBACKEND_H
