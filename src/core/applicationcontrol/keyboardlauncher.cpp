#include "core/applicationcontrol/keyboardlauncher.h"

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QEasingCurve>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>
#include <QtCore/QSettings>
#include <QtCore/QTextCodec>
#include <QtCore/QDebug>

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>

#include <QtWidgets/QApplication>

#include "core/backend/base/abstractkeyboardbackend.h"
#include "core/backend/eventhandlers/keyboardeventhandlerthreadeddecorator.h"
#include "core/backend/keysets/keyboardkeyset.h"
#include "core/backend/keysets/factories/xml/xmlkeyboardkeysetfactory.h"

#include "core/backend/keycodetotextconverting/mapping/directsinglelayoutkeycodeonlytotextconvertmappingstrategy.h"
#include "gui/frontend/keyboard.h"
#include "gui/widgeteventfilters/abstractwidgeteventfilter.h"
#include "gui/animations/widgetslideanimator.h"

#ifdef Q_OS_LINUX
#include "system/linux/x11/backend/keycodetotextconverting/x11keysymskeycodetotextconvertstrategy.h"
#include "system/linux/x11/backend/x11backends/x11customeventhandlerkeyboardbackend.h"
#include "system/linux/x11/backend/eventhandlers/x11xrecordxkbdeventhandler.h"
#include "system/linux/x11/backend/eventhandlers/x11xkbdloopqueryingeventhandler.h"
#endif  // Q_OS_LINUX

namespace qt_keyboard
{

namespace
{
AbstractKeyboardBackend *createKeyboardBackend(const ApplicationConfig *const _pConfig,
                                               QObject *const _parent = Q_NULLPTR);
void processLayoutFile(const QString &_filePath, MappingType *const layoutContainer,
                       MappingType *const shiftedLayoutContainer = Q_NULLPTR);
qreal windowOpacity(int _opacity);
}   // namespace

struct KeyboardLauncher::LauncherPrivate Q_DECL_FINAL
{
    QSharedPointer<const ApplicationConfig> pApplicationConfig;

    QPointer<AbstractKeyboardBackend> pKeyboardBackend;
    QPointer<Keyboard> pKeyboard;


    LauncherPrivate(const QSharedPointer<const ApplicationConfig> &_pConfig)
        : pApplicationConfig(_pConfig) {}
    ~LauncherPrivate() = default;

    void configure() const;
    KeyboardKeySet *createKeySet() const;

private:
    LauncherPrivate(const LauncherPrivate &_other) = default;
    LauncherPrivate(LauncherPrivate &&_other) = default;

    LauncherPrivate &operator=(const LauncherPrivate &_other) = default;
    LauncherPrivate &operator=(LauncherPrivate &&_other) = default;
};

KeyboardLauncher::KeyboardLauncher(const QSharedPointer<const ApplicationConfig> &_pConfig,
                                   QObject *_parent /* = Q_NULLPTR */)    // explicit public
    : QObject(_parent),
      pLauncherPrivate(new LauncherPrivate(_pConfig))
{
    Q_ASSERT(pLauncherPrivate);

    if (!_pConfig)
    {
        throw std::invalid_argument(std::string("Config must not be null in ") + Q_FUNC_INFO);
    }
}

KeyboardLauncher::~KeyboardLauncher()   // virtual public override final
{
    Q_ASSERT(pLauncherPrivate);

    if (pLauncherPrivate -> pKeyboardBackend)
    {
        delete pLauncherPrivate -> pKeyboardBackend;
    }
}

AbstractKeyboardBackend *KeyboardLauncher::keyboardBackend() const   // public
{
    Q_ASSERT(pLauncherPrivate);

    return pLauncherPrivate -> pKeyboardBackend.data();
}

Keyboard *KeyboardLauncher::keyboard() const // public
{
    Q_ASSERT(pLauncherPrivate);

    return pLauncherPrivate -> pKeyboard.data();
}

void KeyboardLauncher::run()   // public
{
    Q_ASSERT(pLauncherPrivate);

    QTimer::singleShot(0, this, &KeyboardLauncher::runAsync);
}

void KeyboardLauncher::runAsync()   // private
{
    Q_ASSERT(pLauncherPrivate && pLauncherPrivate -> pApplicationConfig);

    pLauncherPrivate -> pKeyboardBackend = createKeyboardBackend(pLauncherPrivate -> pApplicationConfig.data());

    if (!pLauncherPrivate -> pKeyboardBackend)
    {
        throw std::runtime_error(std::string("Cannot create keyboard backend in ") + Q_FUNC_INFO);
    }

//    qDebug() << QApplication::font();

    const QSharedPointer<KeyboardConfig> pKeyboardConfig(new KeyboardConfig);
    pKeyboardConfig -> startingLayout = pLauncherPrivate -> pApplicationConfig -> defaultLayout;
    pKeyboardConfig -> startingLayoutIndex = pLauncherPrivate -> pApplicationConfig -> defaultLayoutIndex;
    pKeyboardConfig -> rowSpacing = pLauncherPrivate -> pApplicationConfig -> rowSpacing;
    pKeyboardConfig -> buttonSpacing = pLauncherPrivate -> pApplicationConfig -> buttonSpacing;
    pKeyboardConfig -> isDraggable = pLauncherPrivate -> pApplicationConfig -> draggable;
    pKeyboardConfig -> isX11ShiftBugWorkaroundOn = pLauncherPrivate -> pApplicationConfig -> useX11ShiftBugWorkaround;
    pKeyboardConfig -> toolTipVOffset = pLauncherPrivate -> pApplicationConfig -> toolTipVOffset;

// should be deleted somehow (we use Qt::WA_DeleteOnClose in configure())
    const QSharedPointer<const KeyboardKeySet> lpKeySet(pLauncherPrivate -> createKeySet());
    pLauncherPrivate -> pKeyboard = new Keyboard(pLauncherPrivate -> pKeyboardBackend.data(),
                                                 lpKeySet,
                                                 pKeyboardConfig);

    if (!connect(pLauncherPrivate -> pKeyboard.data(), &Keyboard::closed,
                 qApp,  &QCoreApplication::quit))
    {
        throw std::runtime_error(std::string("Signal closed() connection error in ") + Q_FUNC_INFO);
    }

    pLauncherPrivate -> configure();

    pLauncherPrivate -> pApplicationConfig.clear();

    pLauncherPrivate -> pKeyboardBackend -> findAloneWindow();
    pLauncherPrivate -> pKeyboard -> show();
}



void KeyboardLauncher::LauncherPrivate::configure() const // private::public
{
    static const QString defaultLayoutPath(":/layouts/res/layouts/commonLayout.ini");    // must be absolute

    Q_ASSERT(pApplicationConfig && pKeyboard && pKeyboardBackend);

    pKeyboardBackend -> configureMainWidget(pKeyboard.data()); // must do first
    pKeyboard -> setWindowFlag(Qt::WindowStaysOnTopHint, pApplicationConfig -> onTop);
    pKeyboard -> setAttribute(Qt::WA_DeleteOnClose);
    pKeyboard -> setWindowOpacity(windowOpacity(pApplicationConfig -> opacity));
    pKeyboard -> setFixedSize(pApplicationConfig -> size);
    if (pApplicationConfig -> hideCursor)
    {
        pKeyboard -> setCursor(Qt::BlankCursor);
    }


    if (pApplicationConfig -> isAnimated)
    {
        WidgetSlideAnimator *const pAnimator = new WidgetSlideAnimator(pKeyboard.data());

        pAnimator -> setOutputScreen(pApplicationConfig -> outputScreen);
        QEasingCurve easingCurve(QEasingCurve::OutBack);
        easingCurve.setOvershoot(1.1);
        pAnimator -> setEasingCurve(easingCurve);
        pAnimator -> setDuration(pApplicationConfig -> animationDuration);
        pAnimator -> setWidget(pKeyboard.data());
    }
    else
    {
        const QScreen *const pScreen = QGuiApplication::screens().at(pApplicationConfig -> outputScreen);

        Q_ASSERT(pScreen);

        const QRect screenRect(pScreen -> geometry());
        pKeyboard -> move(screenRect.x() + ((screenRect.width() - pKeyboard -> width()) >> 1),
                          screenRect.y() + (screenRect.height() - pKeyboard -> height()));
    }
    if (pApplicationConfig -> pLayoutNameMapper)
    {
        pKeyboardBackend -> setLayoutNameMapper(pApplicationConfig -> pLayoutNameMapper);
    }

    if (pApplicationConfig -> layoutType != "none")
    {
        QString layoutPath;
        if (pApplicationConfig -> layoutType == "common-no-shift")
        {
            if (!pApplicationConfig -> layouts.isEmpty())
            {
                layoutPath = pApplicationConfig -> layouts.at(0);
                if (!layoutPath.isEmpty())
                {
                    const QFileInfo layoutFileInfo(layoutPath);
                    if (layoutFileInfo.isRelative())
                    {
                        layoutPath = makePathAbsolute(layoutPath);
                    } // -> if (layoutFileInfo.isRelative())
                    if (!checkFile(layoutPath))
                    {
                        qWarning() << "Invalid layout file:" << layoutPath << "...";
                        layoutPath.clear();
                    } // -> if (!checkFile(layoutPath))
                } // -> if (!layoutPath.isEmpty())
            } // -> if (!pApplicationConfig -> layouts.isEmpty())
        } // -> if (pApplicationConfig -> layoutType == "common-no-shift")
        if (layoutPath.isEmpty())
        {

            layoutPath = defaultLayoutPath;
        }
        qt_keyboard::MappingType commonLayout;
        qInfo() << "Using layout:" << layoutPath;
        processLayoutFile(layoutPath, &commonLayout);
        const QSharedPointer<const KeycodeToTextConvertStrategyInterface> pForcedKeyText(
                    new DirectSingleLayoutKeycodeOnlyToTextConvertMappingStrategy(commonLayout));
        pKeyboardBackend -> setForcedKeycodeToText(pForcedKeyText);
    }
}


KeyboardKeySet *KeyboardLauncher::LauncherPrivate::createKeySet() const // private::public
{
    static const QString defaultKeysetPath(":/keysets/res/keysets/default.xml");    // must be absolute

    Q_ASSERT(pApplicationConfig);

    QString keysetPath(pApplicationConfig -> keyset);

    if (!keysetPath.isEmpty())
    {
        QFileInfo keysetFileInfo(keysetPath);
        if (keysetFileInfo.isRelative())
        {
            keysetPath = makePathAbsolute(keysetPath);
        }
        keysetFileInfo.setFile(keysetPath);

        if (!checkFile(keysetFileInfo))
        {
            qWarning() << "Invalid keyset file:" << keysetPath << "Using the default...";
            keysetPath = defaultKeysetPath;
        }
    }
    else // -> if (!keysetPath.isEmpty())
    {
        keysetPath = defaultKeysetPath;
    }
    qDebug() << "Using keyset" << keysetPath << "...";
    const XMLKeyboardKeySetFactory currLayoutFactory(keysetPath);
    return currLayoutFactory.createKeyboardKeySet();
}

namespace
{
AbstractKeyboardBackend *createKeyboardBackend(const ApplicationConfig *const _pConfig,
                                               QObject *const _parent /* = Q_NULLPTR */)
{
#ifdef Q_OS_LINUX

    Q_ASSERT(_pConfig);

    AbstractKeyboardEventHandler *coreEventHandler = Q_NULLPTR;
    switch (_pConfig -> eventHandlerForX11Type)
    {
    case EventHandlerForX11Type::None:
        throw std::invalid_argument(std::string("EventHandlerForX11Type::None is not supported yet: ") + Q_FUNC_INFO);
    case EventHandlerForX11Type::XRecord:
        coreEventHandler = _pConfig -> displayNameX11.isEmpty() ?
                    new X11XRecordXKbdEventHandler :
                    new X11XRecordXKbdEventHandler(_pConfig -> displayNameX11.toLatin1());
        break;
    case EventHandlerForX11Type::Querying:
    {
        const int li_pollInterval = _pConfig -> kbdStatePollInterval > -1 ? _pConfig -> kbdStatePollInterval :
                                                                            defKbdStatePollInterval;
        coreEventHandler = _pConfig -> displayNameX11.isEmpty() ?
                    new X11XKbdLoopQueryingEventHandler(li_pollInterval) :
                    new X11XKbdLoopQueryingEventHandler(_pConfig -> displayNameX11.toLatin1(), li_pollInterval);
        break;
    }
    default:
        throw std::invalid_argument(std::string("Unknown or unsupported EventHandlerForX11Type: ") + Q_FUNC_INFO);
    }
    if (coreEventHandler)
    {
        KeyboardEventHandlerThreadedDecorator *const threadedEventHandler =
                new KeyboardEventHandlerThreadedDecorator(coreEventHandler); // takes ownership
        X11CustomEventHandlerKeyboardBackend *const backend =
                new X11CustomEventHandlerKeyboardBackend(threadedEventHandler, _parent);
        threadedEventHandler -> setParent(backend);
        return backend;
    }
    return Q_NULLPTR;
#else
    return Q_NULLPTR;
#endif // Q_OS_LINUX
}

qreal windowOpacity(int _opacity)
{
    _opacity = _opacity < 0 || _opacity > 100 ? defOpacity :
                                                _opacity;
    return _opacity / qreal(100.0);
}

void processLayoutFile(const QString &_filePath, MappingType *const layoutContainer,
                       MappingType *const shiftedLayoutContainer /* = Q_NULLPTR */)
{
    QSettings settings(_filePath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName(defaultTextCodecName));
    const int keyCount = settings.beginReadArray("Keys");

    if (keyCount > 0)
    {
        Q_ASSERT(layoutContainer);

        for (int i = 0; i < keyCount; ++i)
        {
            settings.setArrayIndex(i);
            bool s;
            const int currKeycode(settings.value("keycode").toInt(&s));
            if (s && currKeycode > 0)
            {
                const QString currText(settings.value("text").toString());
                layoutContainer -> insert(currKeycode, currText);
                if (shiftedLayoutContainer)
                {
                    const QString currShiftText(settings.value("shiftText").toString());
                    shiftedLayoutContainer -> insert(currKeycode, currShiftText);
                }
            }
        }
    }
    settings.endArray(); // Keys
}

} // namespace

} // qt_keyboard
