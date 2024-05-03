#ifndef X11XKBDLOOPQUERYINGEVENTHANDLER_H
#define X11XKBDLOOPQUERYINGEVENTHANDLER_H

/**
 * Supposed to run in a separate thread
**/

#include <QtGlobal>

#ifdef Q_OS_LINUX

#include <memory>

#include "system/linux/x11/backend/eventhandlers/abstractx11xkbdqueryingeventhandler.h"

namespace qt_keyboard
{

class X11XKbdLoopQueryingEventHandler Q_DECL_FINAL : public AbstractX11XKbdQueryingEventHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(X11XKbdLoopQueryingEventHandler)

public:
    explicit X11XKbdLoopQueryingEventHandler(QObject *_parent = Q_NULLPTR);
    explicit X11XKbdLoopQueryingEventHandler(const int _pollingInterval,
                                         QObject *_parent = Q_NULLPTR);
    explicit X11XKbdLoopQueryingEventHandler(const QByteArray &_displayName,
                                         QObject *_parent = Q_NULLPTR);
    X11XKbdLoopQueryingEventHandler(const QByteArray &_displayName,
                                const int _pollingInterval,
                                QObject *_parent = Q_NULLPTR);
    virtual ~X11XKbdLoopQueryingEventHandler() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL;

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
};

} // qt_keyboard

#endif // Q_OS_LINUX

#endif // X11XKBDLOOPQUERYINGEVENTHANDLER_H
