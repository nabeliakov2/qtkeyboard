#include "system/linux/x11/backend/eventhandlers/x11xkbdloopqueryingeventhandler.h"

#ifdef Q_OS_LINUX

#include <QtCore/QThread>
#include <QtCore/QDebug>

#include <X11/XKBlib.h> // Xkb*

#include "X11/keysym.h"

namespace qt_keyboard
{

struct X11XKbdLoopQueryingEventHandler::HandlerPrivate Q_DECL_FINAL // private
{
    const int pollingInterval {200};  // ms

    std::atomic_bool isRunning {false};

    HandlerPrivate() = default;
    HandlerPrivate(const int _pollingInterval)
        : pollingInterval(_pollingInterval) {}
    ~HandlerPrivate() = default;

    HandlerPrivate(const HandlerPrivate &_other) = delete;
    HandlerPrivate(HandlerPrivate &&_other) = delete;
    HandlerPrivate &operator=(const HandlerPrivate &_other) = delete;
    HandlerPrivate &operator=(HandlerPrivate &&_other) = delete;
};

X11XKbdLoopQueryingEventHandler::X11XKbdLoopQueryingEventHandler(QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractX11XKbdQueryingEventHandler(_parent),
      pHandlerPrivate(new HandlerPrivate)
{}

X11XKbdLoopQueryingEventHandler::X11XKbdLoopQueryingEventHandler(const int _pollingInterval,
                                                                 QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractX11XKbdQueryingEventHandler(_parent),
      pHandlerPrivate(new HandlerPrivate(_pollingInterval))
{}

X11XKbdLoopQueryingEventHandler::X11XKbdLoopQueryingEventHandler(const QByteArray &_displayName,
                                                                 QObject *_parent /* = Q_NULLPTR */)  // public
    : AbstractX11XKbdQueryingEventHandler(_displayName, _parent),
      pHandlerPrivate(new HandlerPrivate)
{}

X11XKbdLoopQueryingEventHandler::X11XKbdLoopQueryingEventHandler(const QByteArray &_displayName,
                                                                 const int _pollingInterval,
                                                                 QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractX11XKbdQueryingEventHandler(_displayName, _parent),
      pHandlerPrivate(new HandlerPrivate(_pollingInterval))
{}

X11XKbdLoopQueryingEventHandler::~X11XKbdLoopQueryingEventHandler() Q_DECL_NOEXCEPT // virtual public override final
{
    X11XKbdLoopQueryingEventHandler::stop();
}

int X11XKbdLoopQueryingEventHandler::pollingInterval() const    // public
{
    Q_ASSERT(pHandlerPrivate);

    return pHandlerPrivate -> pollingInterval;  // it's a ctor-time const (no sync is needed)
}

void X11XKbdLoopQueryingEventHandler::start()    // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    pHandlerPrivate -> isRunning.store(true, std::memory_order_release);
    run();
}

void X11XKbdLoopQueryingEventHandler::stop() // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    pHandlerPrivate -> isRunning.store(false, std::memory_order_release);
}

bool X11XKbdLoopQueryingEventHandler::isRunning() const  // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    return pHandlerPrivate -> isRunning.load(std::memory_order_acquire);
}

void X11XKbdLoopQueryingEventHandler::doRun()   // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    while (pHandlerPrivate -> isRunning.load(std::memory_order_acquire))
    {
        poll();

        QThread::currentThread() -> msleep(pHandlerPrivate -> pollingInterval);
    }
    AbstractX11XKbdQueryingEventHandler::stop();
    emit finished();
}


} // qt_keyboard

#endif // Q_OS_LINUX
