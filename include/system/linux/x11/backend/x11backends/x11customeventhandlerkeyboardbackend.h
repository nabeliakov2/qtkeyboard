#ifndef X11CUSTOMEVENTHANDLERKEYBOARDBACKEND_H
#define X11CUSTOMEVENTHANDLERKEYBOARDBACKEND_H

#include <QtGlobal>

#ifdef Q_OS_LINUX

#include <memory>

#include "system/linux/x11/backend/x11backends/x11keyboardbackend.h"

namespace qt_keyboard
{

class AbstractKeyboardEventHandler;

class X11CustomEventHandlerKeyboardBackend Q_DECL_FINAL : public X11KeyboardBackend
{
    Q_OBJECT
    Q_DISABLE_COPY(X11CustomEventHandlerKeyboardBackend)

public:
    explicit X11CustomEventHandlerKeyboardBackend(AbstractKeyboardEventHandler *const _pEventHandler,
                                                  QObject *_parent = Q_NULLPTR);
    virtual ~X11CustomEventHandlerKeyboardBackend() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    struct X11HandlerBackendPrivate;
    const std::unique_ptr<X11HandlerBackendPrivate> pX11HandlerBackendPrivate;

    void init();
};

} // qt_keyboard

#endif // Q_OS_LINUX

#endif // X11CUSTOMEVENTHANDLERKEYBOARDBACKEND_H


