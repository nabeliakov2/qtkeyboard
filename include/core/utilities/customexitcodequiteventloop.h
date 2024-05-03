#ifndef CUSTOMEXITCODEQUITEVENTLOOP_H
#define CUSTOMEXITCODEQUITEVENTLOOP_H

#include <QtCore/QEventLoop>

class CustomExitCodeQuitEventLoop /* final */ : public QEventLoop
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomExitCodeQuitEventLoop)

public:
    explicit CustomExitCodeQuitEventLoop(QObject *_parent = nullptr)
        : QEventLoop(_parent) {}
    explicit CustomExitCodeQuitEventLoop(const int _exitCode, QObject *_parent = nullptr)
        : QEventLoop(_parent), i_exitCode(_exitCode) {}
    virtual ~CustomExitCodeQuitEventLoop() override final = default;

    inline void setCustomExitCode(const int _exitCode) noexcept
    {i_exitCode = _exitCode;}
    inline int customExitCode() const noexcept
    {return i_exitCode;}

public slots:
    void customQuit() {exit(i_exitCode);}
    void additionalCustomQuit() {exit(i_exitCode + 1);}

private:
    int i_exitCode {0};

};

#endif // CUSTOMEXITCODEQUITEVENTLOOP_H
