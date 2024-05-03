#ifndef EXCEPTIONHANDLINGAPPLICATION_H
#define EXCEPTIONHANDLINGAPPLICATION_H

#include <QtWidgets/QApplication>

class ExceptionHandlingApplication : public QApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(ExceptionHandlingApplication)

public:
    explicit ExceptionHandlingApplication(int &_argc, char **_argv)
        : QApplication(_argc, _argv) {}
    virtual ~ExceptionHandlingApplication() = default;

    virtual bool notify(QObject *_receiver, QEvent *_event) Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL;
};

#endif // EXCEPTIONHANDLINGAPPLICATION_H
