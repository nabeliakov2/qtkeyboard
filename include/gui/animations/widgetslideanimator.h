#ifndef WIDGETSLIDEANIMATOR_H
#define WIDGETSLIDEANIMATOR_H

#include <memory>

#include "gui/widgeteventfilters/abstractwidgeteventfilter.h"

namespace qt_keyboard
{

class WidgetSlideAnimator Q_DECL_FINAL: public AbstractWidgetEventFilter
{
    Q_OBJECT
    Q_DISABLE_COPY(WidgetSlideAnimator)

public:
    explicit WidgetSlideAnimator(QObject *_parent = Q_NULLPTR);
    virtual ~WidgetSlideAnimator() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL;

    void setOutputScreen(const int _screen) Q_DECL_NOEXCEPT;
    int outputScreen() const Q_DECL_NOEXCEPT;
    void setDuration(const int _duration) Q_DECL_NOEXCEPT;
    int duration() const Q_DECL_NOEXCEPT;
    void setEasingCurve(const QEasingCurve &_easingCurve) Q_DECL_NOEXCEPT;
    QEasingCurve easingCurve() const Q_DECL_NOEXCEPT;

protected:
/// From AbstractWidgetEventFilter
/// implemented
    virtual bool eventFilter(QObject *_object, QEvent *_event) Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    struct SlideAnimatorPrivate;
    std::unique_ptr<SlideAnimatorPrivate> pSlideAnimatorPrivate;

    void showAnimation() Q_DECL_NOEXCEPT;
    void hideAnimation() Q_DECL_NOEXCEPT;
};

} // qt_keyboard

#endif // WIDGETSLIDEANIMATOR_H
