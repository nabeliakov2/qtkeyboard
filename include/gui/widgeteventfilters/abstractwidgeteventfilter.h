#ifndef ABSTRACTWIDGETEVENTFILTER_H
#define ABSTRACTWIDGETEVENTFILTER_H

#include <QtCore/QObject>
#include <QtCore/QPointer>

#include <QtWidgets/QWidget>

namespace qt_keyboard
{

class AbstractWidgetEventFilter : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractWidgetEventFilter)

public:
    explicit AbstractWidgetEventFilter(QObject *_parent = Q_NULLPTR)
        : QObject(_parent)
    {}
    virtual ~AbstractWidgetEventFilter() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE = default;

    void setWidget(QWidget *const _pWidget);
    inline QWidget *widget() const Q_DECL_NOEXCEPT
    {
        return pWidget.data();
    }
    void unsetWidget();


protected:
/// From QObject
/// for reference
    virtual bool eventFilter(QObject *_object, QEvent *_event) Q_DECL_OVERRIDE = 0;

private:
    QPointer<QWidget> pWidget;

};

} // qt_keyboard

#endif // ABSTRACTWIDGETEVENTFILTER_H
