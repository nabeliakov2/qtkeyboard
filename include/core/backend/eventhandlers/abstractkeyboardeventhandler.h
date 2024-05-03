#ifndef ABSTRACTKEYBOARDEVENTHANDLER_H
#define ABSTRACTKEYBOARDEVENTHANDLER_H

#include <QtCore/QObject>

#include "core/utilities/keyboardutility.h"

namespace qt_keyboard
{

class AbstractKeyboardEventHandler : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractKeyboardEventHandler)

public:
    explicit AbstractKeyboardEventHandler(QObject *_parent = Q_NULLPTR)
        : QObject(_parent) {}
    virtual ~AbstractKeyboardEventHandler() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE = default;

    virtual void start() = 0;
    virtual void stop() = 0;

    virtual bool isRunning() const = 0;

signals:
    void started() const;
    void finished() const;
    void layoutsEvent(const QStringList &_layouts) const;
    void currentLayoutEvent(const int _layoutIndex) const;
    void locksStateEvent(const KeyboardUtility::LocksStateFlags _locksState) const;
    void keyStateEvent(const int _keycode, const bool _pressed) const;

};

} // qt_keyboard

#endif // ABSTRACTKEYBOARDEVENTHANDLER_H
