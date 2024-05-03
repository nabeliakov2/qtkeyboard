#ifndef EVENTPROCESSINGSIGNALWAITER_H
#define EVENTPROCESSINGSIGNALWAITER_H

/**
 * Static class
 *
**/


#include <QtCore/QObject>
#include <QtCore/QEventLoop>

class CustomExitCodeQuitEventLoop;

class EventProcessingSignalWaiter final
{
public:
    static const int timeoutExitCode = 1;
    enum ExitResult {NormalExit = 0, Timeout = timeoutExitCode, Error};

    EventProcessingSignalWaiter() = delete;
    EventProcessingSignalWaiter(const EventProcessingSignalWaiter&) = delete;
    EventProcessingSignalWaiter &operator=(const EventProcessingSignalWaiter&) = delete;
    ~EventProcessingSignalWaiter() = delete;

// throws runtime error on connection errors
    static bool wait(QObject *const sender,
                     const char *const normalSignal,
                     const QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents,
                     const int timeout = -1);
// throws runtime error on connection errors
    static ExitResult waitErrorChecked(QObject *const sender,
                                       const char *const normalSignal,
                                       const char *const errorSignal,
                                       const QEventLoop::ProcessEventsFlags flags = QEventLoop::AllEvents,
                                       const int timeout = -1);

};

#endif // EVENTPROCESSINGSIGNALWAITER_H
