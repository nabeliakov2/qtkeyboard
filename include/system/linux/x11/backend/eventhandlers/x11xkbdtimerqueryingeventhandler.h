#ifndef X11XKBDTIMERQUERYINGEVENTHANDLER_H
#define X11XKBDTIMERQUERYINGEVENTHANDLER_H

/**
 * May be run in the main thread (not tested)
 * Running in a separate thread may require synchronization
**/

#include <QtGlobal>

#ifdef Q_OS_LINUX

#include <memory>

#include "system/linux/x11/backend/eventhandlers/abstractx11xkbdqueryingeventhandler.h"

namespace qt_keyboard
{

class X11XKbdTimerQueryingEventHandler Q_DECL_FINAL : public AbstractX11XKbdQueryingEventHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(X11XKbdTimerQueryingEventHandler)

public:
    static const int defaultPollingInterval {200};

    explicit X11XKbdTimerQueryingEventHandler(QObject *_parent = Q_NULLPTR);
    explicit X11XKbdTimerQueryingEventHandler(const int _pollingInterval,
                                         QObject *_parent = Q_NULLPTR);
    explicit X11XKbdTimerQueryingEventHandler(const QByteArray &_displayName,
                                         QObject *_parent = Q_NULLPTR);
    X11XKbdTimerQueryingEventHandler(const QByteArray &_displayName,
                                const int _pollingInterval,
                                QObject *_parent = Q_NULLPTR);
    virtual ~X11XKbdTimerQueryingEventHandler() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL;

    int pollingInterval() const;

/// From AbstractX11XKbdQueryingEventHandler
/// reimplemented
    virtual void stop() Q_DECL_OVERRIDE Q_DECL_FINAL;
/// From AbstractKeyboardEventHandler
/// implemented
    virtual void start() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool isRunning() const Q_DECL_OVERRIDE Q_DECL_FINAL;

protected:
/// From AbstractX11XKbdQueryingEventHandler
/// implemented
    virtual void doRun() Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    struct HandlerPrivate;
    const std::unique_ptr<HandlerPrivate> pHandlerPrivate;

    void init(int _pollingInterval = defaultPollingInterval);
};

} // qt_keyboard

#endif // Q_OS_LINUX

#endif // X11XKBDTIMERQUERYINGEVENTHANDLER_H
