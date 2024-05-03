#include "core/utilities/eventprocessingsignalwaiter.h"

#include <QtCore/QTimer>
#include <QtCore/QDebug>

#include "core/utilities/customexitcodequiteventloop.h"

namespace
{
EventProcessingSignalWaiter::ExitResult start(QObject *const _sender,
                                              const char *const _normalSignal,
                                              const QEventLoop::ProcessEventsFlags _flags,
                                              const int _timeout,
                                              CustomExitCodeQuitEventLoop &_eventLoop)
{
    QTimer timeoutTimer;

    if (_timeout >= 0)
    {
        timeoutTimer.setSingleShot(true);
        if (!QObject::connect(&timeoutTimer, &QTimer::timeout,
                              &_eventLoop, &CustomExitCodeQuitEventLoop::customQuit))
        {
            throw std::runtime_error(std::string("Timeout timer signal connection error in ") + std::string(Q_FUNC_INFO));
        }
    }
    if (!QObject::connect(_sender, _normalSignal, &_eventLoop, SLOT(quit())))
    {
            throw std::runtime_error(std::string("Signal connection error in ") + std::string(Q_FUNC_INFO));
    }

    if (_timeout >= 0)
    {
        timeoutTimer.start(_timeout);
    }
    const int exitCode {_eventLoop.exec(_flags)};

    return static_cast<EventProcessingSignalWaiter::ExitResult>(exitCode);
}
}   // namespace

bool EventProcessingSignalWaiter::wait(QObject *const _sender,
                                       const char *const _normalSignal,
                                       const QEventLoop::ProcessEventsFlags _flags /* = QEventLoop::AllEvents */,
                                       const int _timeout /* = -1 */)    // static public
{
    CustomExitCodeQuitEventLoop localEventLoop(timeoutExitCode);

    return !static_cast<bool>(start(_sender, _normalSignal, _flags, _timeout, localEventLoop));
}

EventProcessingSignalWaiter::ExitResult EventProcessingSignalWaiter::waitErrorChecked(QObject *const _sender,
                                                                                      const char *const _normalSignal,
                                                                                      const char *const _errorSignal,
                                                                                      const QEventLoop::ProcessEventsFlags _flags /* = QEventLoop::AllEvents */,
                                                                                      const int _timeout /* = -1 */)    // static public
{
    CustomExitCodeQuitEventLoop localEventLoop(timeoutExitCode);

    if (!QObject::connect(_sender, _errorSignal, &localEventLoop, SLOT(additionalCustomQuit())))
    {
        throw std::runtime_error(std::string("Signal connection error in ") + std::string(Q_FUNC_INFO));
    }
    return start(_sender, _normalSignal, _flags, _timeout, localEventLoop);
}

