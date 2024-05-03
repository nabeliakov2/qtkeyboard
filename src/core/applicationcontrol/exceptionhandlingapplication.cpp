#include "core/applicationcontrol/exceptionhandlingapplication.h"

#include <QtCore/QCoreApplication>

#include "core/errorhandling/errorhandler.h"

bool ExceptionHandlingApplication::notify(QObject *_receiver, QEvent *_event) Q_DECL_NOEXCEPT // virtual public ovveride final
{
    try
    {
        return QApplication::notify(_receiver, _event);
    }
    catch (...)
    {
        const int ret = ErrorHandler::handleApplicationExceptions();
        QCoreApplication::exit(ret);
    }
    return false; // must be unreachable
}
