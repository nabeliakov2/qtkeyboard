#include "system/linux/x11/backend/eventhandlers/x11xkbdtimerqueryingeventhandler.h"

#ifdef Q_OS_LINUX

#include <QtCore/QTimer>
#include <QtCore/QDebug>

namespace qt_keyboard
{

struct X11XKbdTimerQueryingEventHandler::HandlerPrivate Q_DECL_FINAL // private
{
    QTimer *timer {Q_NULLPTR};

    HandlerPrivate() = default;
    ~HandlerPrivate() = default;

    HandlerPrivate(const HandlerPrivate &_other) = delete;
    HandlerPrivate(HandlerPrivate &&_other) = delete;
    HandlerPrivate &operator=(const HandlerPrivate &_other) = delete;
    HandlerPrivate &operator=(HandlerPrivate &&_other) = delete;
};

X11XKbdTimerQueryingEventHandler::X11XKbdTimerQueryingEventHandler(QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractX11XKbdQueryingEventHandler(_parent),
      pHandlerPrivate(new HandlerPrivate)
{
    init();
}

X11XKbdTimerQueryingEventHandler::X11XKbdTimerQueryingEventHandler(const int _pollingInterval,
                                                         QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractX11XKbdQueryingEventHandler(_parent),
      pHandlerPrivate(new HandlerPrivate)
{
    init(_pollingInterval);
}

X11XKbdTimerQueryingEventHandler::X11XKbdTimerQueryingEventHandler(const QByteArray &_displayName,
                                                         QObject *_parent /* = Q_NULLPTR */)  // public
    : AbstractX11XKbdQueryingEventHandler(_displayName, _parent),
      pHandlerPrivate(new HandlerPrivate)
{
    init();
}

X11XKbdTimerQueryingEventHandler::X11XKbdTimerQueryingEventHandler(const QByteArray &_displayName,
                                                         const int _pollingInterval,
                                                         QObject *_parent /* = Q_NULLPTR */)  // explicit public
    : AbstractX11XKbdQueryingEventHandler(_displayName, _parent),
      pHandlerPrivate(new HandlerPrivate)
{
    init(_pollingInterval);
}

X11XKbdTimerQueryingEventHandler::~X11XKbdTimerQueryingEventHandler() Q_DECL_NOEXCEPT // virtual public override final
{
    X11XKbdTimerQueryingEventHandler::stop();
}

int X11XKbdTimerQueryingEventHandler::pollingInterval() const    // public
{
    Q_ASSERT(pHandlerPrivate && pHandlerPrivate -> timer);

    return pHandlerPrivate -> timer -> interval();
}

void X11XKbdTimerQueryingEventHandler::init(int _pollingInterval /* = defaultPollingInterval */)    // public
{
    Q_ASSERT(pHandlerPrivate);

    if (_pollingInterval < 0)
    {
        qWarning() << "Invalid polling interval:" << _pollingInterval << "in" << Q_FUNC_INFO << "; using the default...";
        _pollingInterval = defaultPollingInterval;
    }

    pHandlerPrivate -> timer = new QTimer(this);
    pHandlerPrivate -> timer -> setInterval(_pollingInterval);

    if (!connect(pHandlerPrivate -> timer, &QTimer::timeout,
                 this, &X11XKbdTimerQueryingEventHandler::poll))
    {
        throw std::runtime_error(std::string("Signal QTimer::timeout() connection error in ") + Q_FUNC_INFO);
    }
}


void X11XKbdTimerQueryingEventHandler::start()    // virtual public override final
{
    run();
}


void X11XKbdTimerQueryingEventHandler::stop() // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    if (pHandlerPrivate -> timer)
    {
        if (pHandlerPrivate -> timer -> isActive())
        {
            pHandlerPrivate -> timer -> stop();
        }
    }

    AbstractX11XKbdQueryingEventHandler::stop();

    emit finished();
}

bool X11XKbdTimerQueryingEventHandler::isRunning() const  // virtual public override final
{
    Q_ASSERT(pHandlerPrivate);

    return pHandlerPrivate -> timer ? pHandlerPrivate -> timer -> isActive() : false;
}

void X11XKbdTimerQueryingEventHandler::doRun()   // virtual public override final
{
    Q_ASSERT(pHandlerPrivate && pHandlerPrivate -> timer);

    pHandlerPrivate -> timer -> start();
}


} // qt_keyboard

#endif // Q_OS_LINUX
