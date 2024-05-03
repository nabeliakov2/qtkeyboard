#ifndef X11XRECORDXKBDEVENTHANDLER_H
#define X11XRECORDXKBDEVENTHANDLER_H

#include <QtGlobal>

#ifdef Q_OS_LINUX

#include <memory>

#include "core/backend/eventhandlers/abstractkeyboardeventhandler.h"

namespace qt_keyboard
{

class X11XRecordXKbdEventHandler Q_DECL_FINAL : public AbstractKeyboardEventHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(X11XRecordXKbdEventHandler)

public:
    explicit X11XRecordXKbdEventHandler(QObject *_parent = Q_NULLPTR);
    explicit X11XRecordXKbdEventHandler(const QByteArray &_displayName,
                                        QObject *_parent = Q_NULLPTR);
    virtual ~X11XRecordXKbdEventHandler() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL;

/// From AbstractKeyboardEventHandler
/// implemented
    virtual void start() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void stop() Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool isRunning() const Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    friend class XRecordInterceptProcHandler;

    struct HandlerPrivate;
    const std::unique_ptr<HandlerPrivate> pHandlerPrivate;

    void handleCapturedEvent();
// should be run in a separate thread
    void run();
};

} // qt_keyboard

#endif // Q_OS_LINUX

#endif // X11XRECORDXKBDEVENTHANDLER_H

