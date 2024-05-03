#ifndef ABSTRACTX11XKBDQUERYINGEVENTHANDLER_H
#define ABSTRACTX11XKBDQUERYINGEVENTHANDLER_H

#include <QtGlobal>

#ifdef Q_OS_LINUX

#include <memory>

#include "core/backend/eventhandlers/abstractkeyboardeventhandler.h"

namespace qt_keyboard
{


class AbstractX11XKbdQueryingEventHandler : public AbstractKeyboardEventHandler
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractX11XKbdQueryingEventHandler)

public:
    explicit AbstractX11XKbdQueryingEventHandler(QObject *_parent = Q_NULLPTR);
    explicit AbstractX11XKbdQueryingEventHandler(const QByteArray &_displayName,
                                                 QObject *_parent = Q_NULLPTR);
    virtual ~AbstractX11XKbdQueryingEventHandler() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE;


/// From AbstractKeyboardEventHandler
/// implemented
    virtual void stop() Q_DECL_OVERRIDE;
/// for reference
    virtual void start() Q_DECL_OVERRIDE = 0;
    virtual bool isRunning() const Q_DECL_OVERRIDE = 0;

protected:
    void poll();    // TODO: exceptions
    void run();

    virtual void doRun() = 0;

private:
    struct AbstractHandlerPrivate;
    const std::unique_ptr<AbstractHandlerPrivate> pAbstractHandlerPrivate;




};

} // qt_keyboard

#endif // Q_OS_LINUX

#endif // ABSTRACTX11XKBDQUERYINGEVENTHANDLER_H
