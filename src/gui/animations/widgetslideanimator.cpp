#include "gui/animations/widgetslideanimator.h"

#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include <QtGui/QShowEvent>
#include <QtGui/QCloseEvent>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include "core/utilities/eventprocessingsignalwaiter.h"

namespace qt_keyboard
{

namespace
{

const int AnimationExtraWaitDuration {5000}; // ms

QRect screenRect(const int _outputScreen)
{
    const QScreen *const pScreen = QGuiApplication::screens().at(_outputScreen);

    Q_ASSERT(pScreen);

    return pScreen -> geometry();
}

inline QPoint widgetPosShown(const QWidget *const _pWidget, const QRect &_screenRect)
{
    return QPoint(_screenRect.x() + ((_screenRect.width() - _pWidget -> width()) >> 1),
                  _screenRect.y() + (_screenRect.height() - _pWidget -> height()));
}

inline QPoint widgetPosHidden(const QWidget *const _pWidget, const QRect &_screenRect)
{
    return QPoint(_screenRect.x() + ((_screenRect.width() - _pWidget -> width()) >> 1),
                  _screenRect.y() + _screenRect.bottom());
}

QPropertyAnimation *createDefaultAnimation(QObject *const _parent = Q_NULLPTR)
{
    QPropertyAnimation *const pAnimation = new QPropertyAnimation(_parent);
    pAnimation -> setPropertyName("pos");
    pAnimation -> setDuration(1500);
    pAnimation -> setEasingCurve(QEasingCurve::Linear);
    return pAnimation;
}

}

// is never copied
struct WidgetSlideAnimator::SlideAnimatorPrivate Q_DECL_FINAL // private
{
    int i_outputScreen {0};
    QPropertyAnimation *pSlideAnimation {nullptr};
};

WidgetSlideAnimator::WidgetSlideAnimator(QObject *_parent /* = Q_NULLPTR */)
    : AbstractWidgetEventFilter(_parent),
      pSlideAnimatorPrivate(new SlideAnimatorPrivate)
{
    Q_ASSERT(pSlideAnimatorPrivate);

    pSlideAnimatorPrivate -> pSlideAnimation = createDefaultAnimation(this);
}

WidgetSlideAnimator::~WidgetSlideAnimator() Q_DECL_NOEXCEPT // virtual public override final
{}

void WidgetSlideAnimator::setOutputScreen(const int _screen) Q_DECL_NOEXCEPT    // public
{
    Q_ASSERT(pSlideAnimatorPrivate);

    pSlideAnimatorPrivate -> i_outputScreen = _screen;
}

int WidgetSlideAnimator::outputScreen() const Q_DECL_NOEXCEPT   // public
{
    Q_ASSERT(pSlideAnimatorPrivate);

    return pSlideAnimatorPrivate -> i_outputScreen;
}

void WidgetSlideAnimator::setDuration(const int _duration) Q_DECL_NOEXCEPT // public
{
    Q_ASSERT(pSlideAnimatorPrivate);

    if (_duration < 0)
    {
        throw std::invalid_argument(std::string("Duration must be > 0 in ") +
                                                Q_FUNC_INFO);
    }

    if (_duration != pSlideAnimatorPrivate -> pSlideAnimation -> duration())
    {
        pSlideAnimatorPrivate -> pSlideAnimation -> setDuration(_duration);
    }
}

int WidgetSlideAnimator::duration() const Q_DECL_NOEXCEPT   // public
{
    Q_ASSERT(pSlideAnimatorPrivate);

    return pSlideAnimatorPrivate -> pSlideAnimation -> duration();
}

void WidgetSlideAnimator::setEasingCurve(const QEasingCurve &_easingCurve) Q_DECL_NOEXCEPT // public
{
    Q_ASSERT(pSlideAnimatorPrivate);

    if (_easingCurve != pSlideAnimatorPrivate -> pSlideAnimation -> easingCurve())
    {
        pSlideAnimatorPrivate -> pSlideAnimation -> setEasingCurve(_easingCurve);
    }
}

QEasingCurve WidgetSlideAnimator::easingCurve() const Q_DECL_NOEXCEPT // public
{
    Q_ASSERT(pSlideAnimatorPrivate);

    return pSlideAnimatorPrivate -> pSlideAnimation -> easingCurve();
}

bool WidgetSlideAnimator::eventFilter(QObject *_object, QEvent *_event) // virtual public override final
{
    if (_object == widget() && _object)
    {
        if (_event -> type() == QEvent::Show)
        {
            if (!_event -> spontaneous())
            {
                const QRect l_screenRect(screenRect(pSlideAnimatorPrivate -> i_outputScreen));
                const QPoint hPoint(widgetPosHidden(widget(), l_screenRect));
                widget() -> move(hPoint);
                QTimer::singleShot(0, this, &WidgetSlideAnimator::showAnimation);
            }
        }
        else if (_event -> type() == QEvent::Close)
        {
            if (!_event -> spontaneous())
            {
                hideAnimation();
            }
        }
    }
    return false;
}

void WidgetSlideAnimator::showAnimation() Q_DECL_NOEXCEPT   // private
{
    Q_ASSERT(pSlideAnimatorPrivate);

    try
    {
        if (widget())
        {
            const QRect l_screenRect(screenRect(pSlideAnimatorPrivate -> i_outputScreen));


            const QPoint finishAnimationPoint(widgetPosShown(widget(), l_screenRect));
            const QPoint startAnimationPoint(finishAnimationPoint.x(),
                                             l_screenRect.bottom());
            pSlideAnimatorPrivate -> pSlideAnimation -> setTargetObject(widget());
            pSlideAnimatorPrivate -> pSlideAnimation -> setStartValue(startAnimationPoint);
            pSlideAnimatorPrivate -> pSlideAnimation -> setEndValue(finishAnimationPoint);

            pSlideAnimatorPrivate -> pSlideAnimation -> start();
            if (!EventProcessingSignalWaiter::wait(pSlideAnimatorPrivate -> pSlideAnimation,
                                                   SIGNAL(finished()),
                                                   QEventLoop::ExcludeUserInputEvents,
                                                   pSlideAnimatorPrivate -> pSlideAnimation -> duration() + AnimationExtraWaitDuration))
            {
                throw std::runtime_error(std::string("Animation timeout") + std::string(Q_FUNC_INFO));
            }
        }
    }
    catch (const std::exception &_exception)
    {
        qWarning() << "Exception:" << _exception.what() << "in slot" << Q_FUNC_INFO;
    }
    catch (...)
    {
        qWarning() << "Unknown exception in slot" << Q_FUNC_INFO;
    }
}

void WidgetSlideAnimator::hideAnimation() Q_DECL_NOEXCEPT   // private
{
    Q_ASSERT(pSlideAnimatorPrivate);

    try
    {
        if (widget())
        {
            const QRect l_screenRect(screenRect(pSlideAnimatorPrivate -> i_outputScreen));


            const QPoint startAnimationPoint(widget() -> pos());
            const QPoint finishAnimationPoint(widgetPosHidden(widget(), l_screenRect));

            pSlideAnimatorPrivate -> pSlideAnimation -> setTargetObject(widget());
            pSlideAnimatorPrivate -> pSlideAnimation -> setStartValue(startAnimationPoint);
            pSlideAnimatorPrivate -> pSlideAnimation -> setEndValue(finishAnimationPoint);

            pSlideAnimatorPrivate -> pSlideAnimation -> start();
            if (!EventProcessingSignalWaiter::wait(pSlideAnimatorPrivate -> pSlideAnimation,
                                                   SIGNAL(finished()),
                                                   QEventLoop::ExcludeUserInputEvents,
                                                   pSlideAnimatorPrivate -> pSlideAnimation -> duration() + AnimationExtraWaitDuration))
            {
                throw std::runtime_error(std::string("Animation timeout") + std::string(Q_FUNC_INFO));
            }
        }
    }
    catch (const std::exception &_exception)
    {
        qWarning() << "Exception:" << _exception.what() << "in slot" << Q_FUNC_INFO;
    }
    catch (...)
    {
        qWarning() << "Unknown exception in slot" << Q_FUNC_INFO;
    }

}






} // qt_keyboard
