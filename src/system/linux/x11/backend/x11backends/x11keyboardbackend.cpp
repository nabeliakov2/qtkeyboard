#include "system/linux/x11/backend/x11backends/x11keyboardbackend.h"

#ifdef Q_OS_LINUX

#include <QtCore/QSocketNotifier>
#include <QtCore/QTimer>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <QtWidgets/QWidget>


#include <QtX11Extras/QX11Info>

#include <stdexcept>

#include <signal.h>     // signal handling
#include <sys/unistd.h> // ::write()
#include <sys/socket.h> // AF_UNIX SOCK_STREAM
#include <X11/extensions/XTest.h> // XTestFakeKeyEvent
#include <X11/XKBlib.h> // XkbLockGroup

#include "system/linux/x11/backend/keycodetotextconverting/x11keysymskeycodetotextconvertstrategy.h"

namespace qt_keyboard
{

namespace
{
static int sigtermFd[2];

void termSignalHandler(int)
{
    char a = 1;
    ::write(sigtermFd[0], &a, sizeof(a));
}

}   // namespace

struct X11KeyboardBackend::X11KeyboardBackendPrivate Q_DECL_FINAL // private
{
    QSocketNotifier *termSocketNotifier {nullptr};
    QScopedPointer<X11KeySymsKeycodeToTextConvertStrategy> pX11KeyTextStrategy;

    X11KeyboardBackendPrivate()
        : pX11KeyTextStrategy(new X11KeySymsKeycodeToTextConvertStrategy) {}
    ~X11KeyboardBackendPrivate() = default;

private:
    X11KeyboardBackendPrivate(const X11KeyboardBackendPrivate &_other) = delete;
    X11KeyboardBackendPrivate(X11KeyboardBackendPrivate &&_other) = delete;

    X11KeyboardBackendPrivate &operator=(const X11KeyboardBackendPrivate &_other) = delete;
    X11KeyboardBackendPrivate &operator=(X11KeyboardBackendPrivate &&_other) = delete;
};

X11KeyboardBackend::X11KeyboardBackend(QObject *_parent /* = Q_NULLPTR */) // explicit public
    : AbstractKeyboardBackend(_parent),
      pX11Private(new X11KeyboardBackendPrivate)
{
    init();
}

X11KeyboardBackend::~X11KeyboardBackend() Q_DECL_NOEXCEPT // virtual public override
{}

void X11KeyboardBackend::init() // private
{
    Q_ASSERT(pX11Private);

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd))
    {
        qWarning("Couldn't create TERM socketpair in %s", Q_FUNC_INFO);
    }
    pX11Private -> termSocketNotifier = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read, this);
    if (!connect(pX11Private -> termSocketNotifier, &QSocketNotifier::activated,
                 this, &X11KeyboardBackend::handleSigTerm))
    {
        throw std::runtime_error(std::string("Signal connection error in") + Q_FUNC_INFO);
    }
}

void X11KeyboardBackend::handleSigTerm(const int) Q_DECL_NOEXCEPT // private
{
    Q_ASSERT(pX11Private && pX11Private -> termSocketNotifier);

    try
    {
        pX11Private -> termSocketNotifier -> setEnabled(false);

        char tmp;
        ::read(sigtermFd[1], &tmp, sizeof(tmp));

        QTimer::singleShot(0, this, [this]() {emit termRequested();});

        pX11Private -> termSocketNotifier -> setEnabled(true);
    }
    catch (const std::exception &_exception)
    {
        qWarning("Exception: %s in slot %s", _exception.what(), Q_FUNC_INFO);
    }
    catch (...)
    {
        qWarning("Unknown exception in slot %s", Q_FUNC_INFO);
    }
}

void X11KeyboardBackend::findAloneWindow() const   // virtual public override final
{
    unsigned int numkids, i, scrn;
    Window r, p;
    Window *kids=0;
    Window root;
    Display *dipsy=0;
    char *win_name=0;

    dipsy = XOpenDisplay(0);
    if (!dipsy)return;

    scrn = DefaultScreen(dipsy);
    root = RootWindow(dipsy, scrn);

    XQueryTree(dipsy, root, &r, &p, &kids, &numkids);

    for (i = 0; i < numkids;  ++i)
    {
        XFetchName(dipsy, kids[i], &win_name);
        XFree(win_name);
    }
    XCloseDisplay(dipsy);
}


void X11KeyboardBackend::configureMainWidget(QWidget * const _pWidget) const    // virtual public override final
{
    if (!_pWidget)
    {
        throw std::invalid_argument(std::string("Widget cannpot be null in ") + Q_FUNC_INFO);
    }

#ifdef QT590
    _pWidget -> setWindowFlags(static_cast<Qt::WindowType>(Qt::Tool | Qt::X11BypassWindowManagerHint));
#else
    _pWidget -> setWindowFlags(Qt::Tool | Qt::X11BypassWindowManagerHint);
#endif

    struct sigaction term;
    term.sa_handler = termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;
    if (sigaction(SIGTERM, &term, 0) > 0)
    {
        // throw std::runtime_error(std::string("Error SIGTERM handler configuring in ") + Q_FUNC_INFO);

        qWarning("Error SIGTERM handler configuring in %s", Q_FUNC_INFO);
    }
}

void X11KeyboardBackend::simulateKeyEvent(const int _keycode, const bool _pressed) const // virtual public override final
{
    Display *const display = QX11Info::display();

    Q_ASSERT(display);

    XTestFakeKeyEvent(display, _keycode, _pressed, 0);
    XFlush(display);

    QCoreApplication::processEvents(QEventLoop::AllEvents, KeyboardUtility::processEventsMaxTime);
}

void X11KeyboardBackend::doSwitchToLayout(const int _layoutIndex) // virtual public override final
{
    Display *const display = QX11Info::display();

    Q_ASSERT(display);

    if (!XkbLockGroup(display, XkbUseCoreKbd, _layoutIndex))
    {
        throw std::runtime_error(std::string("Cannot switch to layout ") + std::to_string(_layoutIndex) + " at " + Q_FUNC_INFO);
    }
    XFlush(display);
}

QString X11KeyboardBackend::keyText(const int _keycode,
                                    const int _layoutIndex,
                                    const bool _shift /* = false */,
                                    const bool _caps /* = false */) const  // virtual public override final
{
    Q_ASSERT(pX11Private && pX11Private -> pX11KeyTextStrategy);

    return pX11Private -> pX11KeyTextStrategy -> keyText(_keycode, _layoutIndex, _shift, _caps);
}

QString X11KeyboardBackend::keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                           const bool _shift /* = false */, const bool _caps /* = false */) const // virtual public override final
{
    Q_ASSERT(pX11Private && pX11Private -> pX11KeyTextStrategy);

    return pX11Private -> pX11KeyTextStrategy -> keyTextChecked(_hasKeyTextReturn, _keycode, _layoutIndex, _shift, _caps);
}

bool X11KeyboardBackend::hasKeyText(const int _keycode, const int _layoutIndex, const bool _shift /* = false */) const // virtual public override final
{
    Q_ASSERT(pX11Private && pX11Private -> pX11KeyTextStrategy);

    return pX11Private -> pX11KeyTextStrategy -> hasKeyText(_keycode, _layoutIndex, _shift);
}



} // qt_keyboard

#endif  // Q_OS_LINUX

