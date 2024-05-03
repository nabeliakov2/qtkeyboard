#ifndef KEYBOARDEVENTHANDLERTHREADEDDECORATOR_H
#define KEYBOARDEVENTHANDLERTHREADEDDECORATOR_H

#include <QtCore/QThread>

#include <memory>

#include "core/backend/eventhandlers/abstractkeyboardeventhandler.h"

namespace qt_keyboard
{

class KeyboardEventHandlerThreadedDecorator Q_DECL_FINAL : public AbstractKeyboardEventHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(KeyboardEventHandlerThreadedDecorator)

public:
    explicit KeyboardEventHandlerThreadedDecorator(AbstractKeyboardEventHandler *const _pEventHandler,
                                                   QObject *_parent = Q_NULLPTR);
    KeyboardEventHandlerThreadedDecorator(AbstractKeyboardEventHandler *const _pEventHandler,
                                          const QThread::Priority _priority,
                                          QObject *_parent = Q_NULLPTR);
    virtual ~KeyboardEventHandlerThreadedDecorator() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL;

    AbstractKeyboardEventHandler *eventHandler() const;

    void setThreadPriority(const QThread::Priority _priority);
    QThread::Priority threadPriority() const;

/// From AbstractKeyboardEventHandler
/// implemented
    virtual void start() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void stop() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool isRunning() const Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    struct DecoratorPrivate;
    const std::unique_ptr<DecoratorPrivate> pDecoratorPrivate;

    void init();
};



} // qt_keyboard

#endif // KEYBOARDEVENTHANDLERTHREADEDDECORATOR_H
