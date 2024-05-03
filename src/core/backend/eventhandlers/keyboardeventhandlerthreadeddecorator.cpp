#include "core/backend/eventhandlers/keyboardeventhandlerthreadeddecorator.h"

#include <QtCore/QPointer>

namespace qt_keyboard
{

struct KeyboardEventHandlerThreadedDecorator::DecoratorPrivate Q_DECL_FINAL // private
{
    QThread *pThread {Q_NULLPTR};

    QPointer<AbstractKeyboardEventHandler> pHandler;
    QThread::Priority threadPriority {QThread::NormalPriority};

    explicit DecoratorPrivate(AbstractKeyboardEventHandler *const _pEventHandler)
        : pHandler(_pEventHandler) {}
    explicit DecoratorPrivate(AbstractKeyboardEventHandler *const _pEventHandler,
                              const QThread::Priority _priority)
        : pHandler(_pEventHandler),
          threadPriority(_priority) {}
    ~DecoratorPrivate() = default;

    DecoratorPrivate(const DecoratorPrivate &_other) = delete;
    DecoratorPrivate(DecoratorPrivate &&_other) = delete;
    DecoratorPrivate &operator=(const DecoratorPrivate &_other) = delete;
    DecoratorPrivate &operator=(DecoratorPrivate &&_other) = delete;
};

KeyboardEventHandlerThreadedDecorator::KeyboardEventHandlerThreadedDecorator(AbstractKeyboardEventHandler *const _pEventHandler,
                                                                             QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractKeyboardEventHandler(_parent),
      pDecoratorPrivate(new DecoratorPrivate(_pEventHandler))
{
    init();
}

KeyboardEventHandlerThreadedDecorator::KeyboardEventHandlerThreadedDecorator(AbstractKeyboardEventHandler *const _pEventHandler,
                                                                             const QThread::Priority _priority,
                                                                             QObject *_parent /* = Q_NULLPTR */)  // public
    : AbstractKeyboardEventHandler(_parent),
      pDecoratorPrivate(new DecoratorPrivate(_pEventHandler, _priority))
{
    init();
}

KeyboardEventHandlerThreadedDecorator::~KeyboardEventHandlerThreadedDecorator() Q_DECL_NOEXCEPT    // virtual public override final
{
    Q_ASSERT(pDecoratorPrivate);

    if (isRunning())
    {
        stop();
    }
    if (pDecoratorPrivate)
    {
        pDecoratorPrivate -> pHandler -> deleteLater();
    }
}

AbstractKeyboardEventHandler *KeyboardEventHandlerThreadedDecorator::eventHandler() const   // public
{
    Q_ASSERT(pDecoratorPrivate);

    return pDecoratorPrivate -> pHandler.data();
}

void KeyboardEventHandlerThreadedDecorator::init()    // private
{
    Q_ASSERT(pDecoratorPrivate);

    if (!pDecoratorPrivate -> pHandler)
    {
        throw std::invalid_argument(std::string("pEventHandler cannot be null in ") + Q_FUNC_INFO);
    }

    if (pDecoratorPrivate -> pHandler -> isRunning())
    {
        pDecoratorPrivate -> pHandler -> stop();
    }

    pDecoratorPrivate -> pThread = new QThread(this);
    pDecoratorPrivate -> pHandler -> moveToThread(pDecoratorPrivate -> pThread);

    if (!connect(pDecoratorPrivate -> pHandler, &AbstractKeyboardEventHandler::started,
                 this, &AbstractKeyboardEventHandler::started, Qt::QueuedConnection))
    {
        throw std::runtime_error(std::string("Signal started() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pDecoratorPrivate -> pHandler, &AbstractKeyboardEventHandler::layoutsEvent,
                 this, &AbstractKeyboardEventHandler::layoutsEvent, Qt::QueuedConnection))
    {
        throw std::runtime_error(std::string("Signal layoutsEvent() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pDecoratorPrivate -> pHandler, &AbstractKeyboardEventHandler::locksStateEvent,
                 this, &AbstractKeyboardEventHandler::locksStateEvent, Qt::QueuedConnection))
    {
        throw std::runtime_error(std::string("Signal indicatorsStateEvent() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pDecoratorPrivate -> pHandler, &AbstractKeyboardEventHandler::currentLayoutEvent,
                 this, &AbstractKeyboardEventHandler::currentLayoutEvent, Qt::QueuedConnection))
    {
        throw std::runtime_error(std::string("Signal indicatorsStateEvent() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pDecoratorPrivate -> pHandler, &AbstractKeyboardEventHandler::keyStateEvent,
                 this, &AbstractKeyboardEventHandler::keyStateEvent, Qt::QueuedConnection))
    {
        throw std::runtime_error(std::string("Signal indicatorsStateEvent() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pDecoratorPrivate -> pHandler, &AbstractKeyboardEventHandler::finished,
                 pDecoratorPrivate -> pThread, &QThread::quit, Qt::QueuedConnection))
    {
        throw std::runtime_error(std::string("Signal indicatorsStateEvent() connection error in ") + Q_FUNC_INFO);
    }
    if (!connect(pDecoratorPrivate -> pThread, &QThread::started,
                 pDecoratorPrivate -> pHandler, &AbstractKeyboardEventHandler::start, Qt::QueuedConnection))
    {
        throw std::runtime_error(std::string("Signal QThread::started() connection error in ") + Q_FUNC_INFO);
    }

}

void KeyboardEventHandlerThreadedDecorator::setThreadPriority(const QThread::Priority _priority)  // public
{
    Q_ASSERT(pDecoratorPrivate && pDecoratorPrivate -> pThread);

    pDecoratorPrivate -> threadPriority = _priority;
    if (pDecoratorPrivate -> pThread -> isRunning())
    {
        pDecoratorPrivate -> pThread -> setPriority(_priority);
    }
}

QThread::Priority KeyboardEventHandlerThreadedDecorator::threadPriority() const   // public
{
    Q_ASSERT(pDecoratorPrivate && pDecoratorPrivate -> pThread);

    return pDecoratorPrivate -> pThread -> isRunning() ? pDecoratorPrivate -> pThread -> priority() :
                                                         pDecoratorPrivate -> threadPriority;
}

void KeyboardEventHandlerThreadedDecorator::start()   // virtual public override final
{
    Q_ASSERT(pDecoratorPrivate);

    pDecoratorPrivate -> pThread -> start(pDecoratorPrivate -> threadPriority);
}

void KeyboardEventHandlerThreadedDecorator::stop()    // virtual public override final
{
    Q_ASSERT(pDecoratorPrivate);

    pDecoratorPrivate -> pHandler -> stop();
    pDecoratorPrivate -> pThread -> quit();
    pDecoratorPrivate -> pThread -> wait();
}

bool KeyboardEventHandlerThreadedDecorator::isRunning() const // virtual public override final
{
    Q_ASSERT(pDecoratorPrivate && pDecoratorPrivate -> pHandler);

    return pDecoratorPrivate -> pHandler -> isRunning();
}

} // qt_keyboard
