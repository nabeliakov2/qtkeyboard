#include <QtCore/QTextCodec>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDebug>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QRegularExpression>
#include <QtCore/QSharedPointer>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>

#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QImageReader>

#include "core/applicationcontrol/exceptionhandlingapplication.h"
#include "core/applicationcontrol/keyboardlauncher.h"
#include "core/errorhandling/errorhandler.h"

namespace
{
const QString defaultQSSPath(":/config/res/config/qtkeyboard.qss"); // must be absolute
qt_keyboard::ApplicationConfig *getConfiguration(const QCoreApplication &_application);
} // namespace


int main(int argc, char *argv[])
{
    int ret = 0;
    try
    {
        QString cleanupCommand;
        ExceptionHandlingApplication app(argc, argv);

        app.setApplicationName("QtKeyboard");
        app.setOrganizationName("Oboronsoft");
        app.setOrganizationDomain("oboronsoft.ru");
        app.setApplicationVersion(QString(APPLICATION_VERSION));
        app.setQuitOnLastWindowClosed(true);



        QTextCodec::setCodecForLocale(QTextCodec::codecForName(qt_keyboard::defaultTextCodecName));

        QSharedPointer<const qt_keyboard::ApplicationConfig> pAppConfig(getConfiguration(app));
        cleanupCommand = pAppConfig -> cleanupCommand;
        QString l_qssPath(pAppConfig -> qssPath);
        if (!l_qssPath.isEmpty())
        {
            QFileInfo qssFileInfo(l_qssPath);
            if (qssFileInfo.isRelative())
            {
                l_qssPath = qt_keyboard::makePathAbsolute(l_qssPath);
            }
            qssFileInfo.setFile(l_qssPath);
            if (!qt_keyboard::checkFile(qssFileInfo))
            {
                qWarning() << "Invalid QSS file" << l_qssPath << "; using the default...";
                l_qssPath = defaultQSSPath;
            }

            QFile qssFile(l_qssPath);
            if (qssFile.open(QFile::ReadOnly | QFile::Text))
            {
                qInfo() << "Using QSS" << l_qssPath << "...";
                app.setStyleSheet(qssFile.readAll());
                qssFile.close();
            }
            else
            {
                qWarning() << "Cannot open QSS file" << pAppConfig -> qssPath << "for reading.";
            }
        }

        qt_keyboard::KeyboardLauncher keyboardLauncher(pAppConfig);
        keyboardLauncher.run();
        pAppConfig.clear(); // Config is needed only for configuring
        ret = app.exec();

        if (!cleanupCommand.isEmpty())
        {
            if (!QProcess::startDetached(cleanupCommand))
            {
                qWarning() << "Could not start cleanup command: " << cleanupCommand << ".";
            }
        }
    }
    catch (...)
    {
        ret = ErrorHandler::handleApplicationExceptions();
    }


    return ret;
}

namespace
{
const QCommandLineOption configPathOption({"c", "config"},
                                          QObject::tr("Path to a config <file>."),
                                          "file");
const QCommandLineOption qssPathOption({"q", "qss"},
                                       QObject::tr("Path to a qss <file>."),
                                       "file");
const QCommandLineOption opacityOption({"o", "opacity"},
                                       QObject::tr("Keyboard widget <opacity>."),
                                       "opacity");
const QCommandLineOption defaultLayoutOption({"l", "layout"},
                                         QObject::tr("Default layout <name>."),
                                         "name");
const QCommandLineOption defaultLayoutIndexOption({"i", "index"},
                                                  QObject::tr("Default layout <index>."),
                                                  "index");

QString processPath(QString &_path, const QString &_defaultFilePath, const QString &_name = {})
{
    bool usingDefault = false;
    if (_path.isEmpty())
    {
        qWarning() << "Path is empty (" << _name << "); using the default...";
        _path = _defaultFilePath;
        usingDefault = true;
    }
    if (_path.isEmpty()) return {};

    QFileInfo fileInfo(_path);

    Q_ASSERT (!usingDefault || fileInfo.isAbsolute());

    if (fileInfo.isRelative())
    {
        _path = qt_keyboard::makePathAbsolute(_path);
    }
    if (!usingDefault)
    {
        fileInfo.setFile(_path);
        if (!qt_keyboard::checkFile(fileInfo))
        {
            qWarning() << "Invalid file (" << _name << "):" << _path << "; using the default...";
            _path = _defaultFilePath;
        }
        else
        {
            return _path;
        }
    }
    if (_path.isEmpty()) return {};
    fileInfo.setFile(_path);
    if (!qt_keyboard::checkFile(fileInfo))
    {
        qWarning() << "No" << _name << "file found!";
        _path.clear();
    }
    return _path;
}

bool addOption(QCommandLineParser &_parser, const QCommandLineOption &_option)
{
    if (!_parser.addOption(_option))
    {
        qWarning() << "Warning: Cannot add option" << _option.names().at(0) << ".";
        return false;
    }
    return true;
}

void processSettings(const QString &_configPath, qt_keyboard::ApplicationConfig *const _pApplicationConfig);
void processCommandLine(const QCommandLineParser &_cliParser, qt_keyboard::ApplicationConfig *const _pApplicationConfig);
void getSize(const QSettings &_settings, qt_keyboard::ApplicationConfig *const _pApplicationConfig);

qt_keyboard::ApplicationConfig *getConfiguration(const QCoreApplication &_application)
{
    static const QString defaultExtConfigFileName("qtkeyboard.cfg"); // must be absolute
    static const QString defaultConfigFilePath(":/config/res/config/qtkeyboard.cfg"); // must be absolute

    QCommandLineParser cliParser;
    cliParser.setApplicationDescription(QCoreApplication::applicationName());
    cliParser.addHelpOption();
    cliParser.addVersionOption();
    addOption(cliParser, configPathOption);
    addOption(cliParser, qssPathOption);
    addOption(cliParser, opacityOption);
    addOption(cliParser, defaultLayoutOption);
    addOption(cliParser, defaultLayoutIndexOption);

    cliParser.process(_application);

    QString configPath(defaultConfigFilePath);
    if (cliParser.isSet(configPathOption))
    {
        configPath = cliParser.value(configPathOption);

        processPath(configPath, defaultConfigFilePath, "config-file");
        if (configPath.isEmpty())
        {
            qWarning() << "Config file not found!";
        }
    }
    else
    {
        QString defExtConfigFilePath(defaultExtConfigFileName);
        const QFileInfo defExtConfigFileInfo(defExtConfigFilePath);
        if (defExtConfigFileInfo.isRelative())
        {
            defExtConfigFilePath = qt_keyboard::makePathAbsolute(defExtConfigFilePath);
        }
        if (qt_keyboard::checkFile(defExtConfigFilePath))
        {
            configPath = defExtConfigFilePath;
        }
        else
        {
            qWarning() << "Invalid config file; Using the default...";
        }
    }

    qt_keyboard::ApplicationConfig *const pApplicationConfig = new qt_keyboard::ApplicationConfig;
    processSettings(configPath, pApplicationConfig);
    processCommandLine(cliParser, pApplicationConfig);
    return pApplicationConfig;
}

void processSettings(const QString &_configPath, qt_keyboard::ApplicationConfig *const _pApplicationConfig)
{
    Q_ASSERT(_pApplicationConfig);

    bool s;
    qInfo() << "Using config:" << _configPath << "...";
    QSettings settings(_configPath, QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName(qt_keyboard::defaultTextCodecName));
    settings.beginGroup("Core");
    const QStringList &coreKeys(settings.childKeys());
    if (coreKeys.contains("qssPath"))
    {
        _pApplicationConfig -> qssPath = settings.value("qssPath").toString();
    }
    _pApplicationConfig -> cleanupCommand = settings.value("cleanupCommand").toString();
    if (coreKeys.contains("defaultLayout"))
    {
        const QString defaultLayout(settings.value("defaultLayout").toString());
        if (!defaultLayout.isEmpty())
        {
            _pApplicationConfig -> defaultLayout = defaultLayout;
        }
    }
    int intValueTest = settings.value("defaultLayoutIndex").toInt(&s);
    if (s && intValueTest > -1)
    {
        _pApplicationConfig -> defaultLayoutIndex = intValueTest;
    }
    if (coreKeys.contains("keyset"))
    {
        const QString keyset(settings.value("keyset").toString());
        if (!keyset.isEmpty())
        {
            _pApplicationConfig -> keyset  = keyset;
        }
        else
        {
            qWarning() << "Keyset path in config file is empty.";
        }
    }
    if (coreKeys.contains("layoutType"))
    {
        QString layoutType(settings.value("layoutType").toString());
        if (!layoutType.isEmpty())
        {
            _pApplicationConfig -> layoutType = layoutType;
        }
    }
    if (coreKeys.contains("draggable"))
    {
        _pApplicationConfig -> draggable = settings.value("draggable", true).toBool();
    }


    settings.endGroup();    // Core

    if (!_pApplicationConfig -> layoutType.isEmpty())
    {
        const int layoutCount = settings.beginReadArray("Layouts");
        if (layoutCount > 0)
        {
            _pApplicationConfig -> layouts.clear();
            _pApplicationConfig -> layouts.reserve(layoutCount);
            for (int i = 0; i < layoutCount; ++i)
            {
                settings.setArrayIndex(i);
                const QString currPath(settings.value("path").toString());
                if (!currPath.isEmpty())
                {
                    _pApplicationConfig -> layouts.push_back(currPath);
                }
            }
        }
        settings.endArray(); // Layouts
    }

    const int mappingsCount = settings.beginReadArray("LayoutNameMapping");
    if (mappingsCount > 0)
    {
        const QSharedPointer<qt_keyboard::LayoutNameMapper> lpLayoutNameMapper(new qt_keyboard::LayoutNameMapper);
        for (int i = 0; i < mappingsCount; ++i)
        {
            settings.setArrayIndex(i);
            const QString currSrc(settings.value("src").toString());
            const QString currMapped(settings.value("mapped").toString());


            lpLayoutNameMapper -> insert(currSrc, currMapped);
        }
        _pApplicationConfig -> pLayoutNameMapper = lpLayoutNameMapper;
    }
    settings.endArray(); // LayoutNameMapping


    settings.beginGroup("View");
    const QStringList &viewKeys(settings.childKeys());
    if (viewKeys.contains("onTop"))
    {
        _pApplicationConfig -> onTop = settings.value("onTop", true).toBool();
    }

    intValueTest = settings.value("opacity").toInt(&s);
    if (s && intValueTest > -1 && intValueTest < 101)
    {
        _pApplicationConfig -> opacity = intValueTest;
    }

    if (viewKeys.contains("outputScreen"))
    {
        const QList<QScreen*> &l_screens(QGuiApplication::screens());
        intValueTest = settings.value("outputScreen").toInt(&s);
        if (s && intValueTest > -1 &&
                intValueTest < l_screens.count())
        {
            _pApplicationConfig -> outputScreen = intValueTest;
        }
        else
        {
            qWarning() << "Invalid output screen in config; using the default...";
        }
    }
    getSize(settings, _pApplicationConfig);
    if (viewKeys.contains("hideCursor"))
    {
        _pApplicationConfig -> hideCursor = settings.value("hideCursor", false).toBool();
    }
    if (viewKeys.contains("isAnimated"))
    {
        _pApplicationConfig -> isAnimated = settings.value("isAnimated", true).toBool();
    }
    intValueTest = settings.value("animationDuration").toInt(&s);
    if (s && intValueTest > 0)
    {
        _pApplicationConfig -> animationDuration = intValueTest;
    }
    else
    {
        qWarning() << "Invalid animation duration in config; using the default...";
    }
    intValueTest = settings.value("rowSpacing").toInt(&s);
    if (s && intValueTest > 0)
    {
        _pApplicationConfig -> rowSpacing = intValueTest;
    }
    intValueTest = settings.value("buttonSpacing").toInt(&s);
    if (s && intValueTest > 0)
    {
        _pApplicationConfig -> buttonSpacing = intValueTest;
    }
    intValueTest = settings.value("toolTipVerticalOffset").toInt(&s);
    if (s)
    {
        _pApplicationConfig -> toolTipVOffset = intValueTest;
    }
    settings.endGroup();    // View

    settings.beginGroup("System");
    const QStringList &systemKeys(settings.childKeys());
    if (systemKeys.contains("displayNameX11"))
    {
         const QString displayNameX11Str(settings.value("displayNameX11").toString());
         if (!displayNameX11Str.isEmpty())
         {
             _pApplicationConfig -> displayNameX11 = displayNameX11Str;
         }
    }
    if (systemKeys.contains("useX11ShiftBugWorkaround"))
    {
         _pApplicationConfig -> useX11ShiftBugWorkaround = settings.value("useX11ShiftBugWorkaround", true).toBool();
    }
    if (systemKeys.contains("eventHandlerForX11"))
    {
        const QString eventHandlerForX11Str(settings.value("eventHandlerForX11").toString());
        if (!eventHandlerForX11Str.isEmpty())
        {
            if (!eventHandlerForX11Str.compare(QObject::tr("none"), Qt::CaseInsensitive))
            {
                _pApplicationConfig -> eventHandlerForX11Type = qt_keyboard::EventHandlerForX11Type::None;
            }
            else if (!eventHandlerForX11Str.compare(QObject::tr("xrecord"), Qt::CaseInsensitive))
            {
                _pApplicationConfig -> eventHandlerForX11Type = qt_keyboard::EventHandlerForX11Type::XRecord;
            }
            else if (!eventHandlerForX11Str.compare(QObject::tr("querying"), Qt::CaseInsensitive))
            {
                _pApplicationConfig -> eventHandlerForX11Type = qt_keyboard::EventHandlerForX11Type::Querying;
            }
            else
            {
                qWarning() << "Unknown eventHandlerForX11Type:" << eventHandlerForX11Str << ".";
            }
        }
    }
    if (systemKeys.contains("kbdStatePollInterval"))
    {
        intValueTest = settings.value("kbdStatePollInterval").toInt(&s);
        if (s && intValueTest > -1)
        {
            _pApplicationConfig -> kbdStatePollInterval = intValueTest;
        }
        else
        {
            qWarning() << "Invalid poll interval in config:" << intValueTest << "; using the default...";
        }
    }
    settings.endGroup();    // System
}

void processCommandLine(const QCommandLineParser &_cliParser, qt_keyboard::ApplicationConfig *const _pApplicationConfig)
{
    Q_ASSERT(_pApplicationConfig);

    if (_cliParser.isSet(qssPathOption))
    {
        const QString l_qssPath(_cliParser.value(qssPathOption));
        if (qt_keyboard::checkFile(l_qssPath))
        {
            _pApplicationConfig -> qssPath = l_qssPath;
        }
        else
        {
            qWarning() << "Invalid QSS file in command line:" << l_qssPath << ".";
        }
    }
    bool s;
    if (_cliParser.isSet(defaultLayoutOption))
    {
        const QString l_defaultLayout(_cliParser.value(defaultLayoutOption));
        if (!l_defaultLayout.isEmpty())
        {
            _pApplicationConfig -> defaultLayout = l_defaultLayout;
        }
        else
        {
            qWarning() << "Empty default layout in command line.";
        }
    }
    if (_cliParser.isSet(defaultLayoutIndexOption))
    {
        const int l_defaultLayoutIndex = _cliParser.value(defaultLayoutIndexOption).toInt(&s);

        if (s && l_defaultLayoutIndex > -1)
        {
            _pApplicationConfig -> defaultLayoutIndex = l_defaultLayoutIndex;
        }
        else
        {
            qWarning() << "Incorrect default layout index in command line:" << l_defaultLayoutIndex << ".";
        }
    }
    if (_cliParser.isSet(opacityOption))
    {
        const int l_opacity = _cliParser.value(opacityOption).toInt(&s);
        if (s && l_opacity > -1 && l_opacity < 101)
        {
            _pApplicationConfig -> opacity = l_opacity;
        }
        else
        {
            qWarning() << "Incorrect opacity in command line:" << l_opacity << ".";
        }
    }
}

int getSizePart(const QString &_sStr, const QString &defSStr, const QString &_name);

void getSize(const QSettings &_settings, qt_keyboard::ApplicationConfig *const _pApplicationConfig)
{
    static const QString defWidthStr {"100"};
    static const QString defHeightStr {"20"};

    const QStringList &keys(_settings.childKeys());
    if (keys.contains("useTextureSize"))
    {
        QString useTextureSize(_settings.value("useTextureSize").toString());
        if (!useTextureSize.isEmpty())
        {
            const QFileInfo textureInfo(useTextureSize);
            if (textureInfo.isRelative())
            {
                useTextureSize = qt_keyboard::makePathAbsolute(useTextureSize);
            }
            if (qt_keyboard::checkFile(useTextureSize) &&
                    !QImageReader::imageFormat(useTextureSize).isEmpty())
            {
                const QImageReader imageReader(useTextureSize);
                if (imageReader.canRead())
                {
                    const QSize l_size(imageReader.size());
                    if (l_size.isValid())
                    {
                        _pApplicationConfig -> size = l_size;
                        return;
                    }   // -> if (l_size.isValid())
                    else
                    {
                        qWarning() << "Invalid size of texture in config.";
                    }
                } // -> if (imageReader.canRead())
                else
                {
                    qWarning() << "Can't read texture file:" << useTextureSize << ".";
                }
            } // -> if (qt_keyboard::checkFile(useTextureSize) && !QImageReader::imageFormat(useTextureSize).isEmpty())
            else
            {
                qWarning() << "Invalid texture file in config:" << useTextureSize << ".";
            }
        } // -> if (!useTextureSize.isEmpty())
    } // -> if (keys.contains("useTextureSize"))
 // size isn't set here
    if (keys.contains("relativeWidth"))
    {
        _pApplicationConfig -> relativeWidth = _settings.value("relativeWidth", true).toBool();
    }
    if (keys.contains("relativeHeight"))
    {
        _pApplicationConfig -> relativeHeight = _settings.value("relativeHeight", true).toBool();
    }
    int tWidth = _pApplicationConfig -> size.width();
    int tHeight = _pApplicationConfig -> size.height();
    if (keys.contains("width"))
    {
        QString widthStr(_settings.value("width", defWidthStr).toString());
        tWidth = getSizePart(widthStr, defWidthStr, "width");
    }
    if (keys.contains("height"))
    {
        QString heightStr(_settings.value("height", defHeightStr).toString());
        tHeight = getSizePart(heightStr, defHeightStr, "height");
    }
    if (_pApplicationConfig -> relativeWidth || _pApplicationConfig -> relativeHeight)
    {
        Q_ASSERT(_pApplicationConfig -> outputScreen > -1);

        const QScreen *const pCurrentScreen = QGuiApplication::screens().at(_pApplicationConfig -> outputScreen);

        Q_ASSERT(pCurrentScreen);

        const QSize screenSize(pCurrentScreen -> size());
        if (_pApplicationConfig -> relativeWidth)
        {
            _pApplicationConfig -> size.setWidth(static_cast<int>(screenSize.width() * (qreal(tWidth) / 100)));
        }
        else
        {
            _pApplicationConfig -> size.setWidth(tWidth);
        }
        if (_pApplicationConfig -> relativeHeight)
        {
            _pApplicationConfig -> size.setHeight(static_cast<int>(screenSize.height() * (qreal(tHeight) / 100)));
        }
        else
        {
            _pApplicationConfig -> size.setHeight(tHeight);
        }

    }
    else    // ->  if (_pApplicationConfig -> relativeWidth || pApplicationConfig -> relativeHeight)
    {
        _pApplicationConfig -> size.setWidth(tWidth);
        _pApplicationConfig -> size.setHeight(tHeight);
    }
}


int getSizePart(const QString &_sStr, const QString &defSStr, const QString &_name)
{
    static const QRegularExpression SizeElementPattern("^\\d{1,4}$");

    QString l_str(_sStr);
    auto onError = [&_name, &l_str, &defSStr]()
    {
        qWarning() << "Invalid" << _name << ":" << l_str << "in config; using the default...";
        l_str = defSStr;
    };

    auto check = [&l_str, &onError]() -> int
    {
        bool s;
        const int rv = l_str.toInt(&s);
        if (!s || rv < 1)
        {
            onError();
        }
        return rv;
    };

    if (!SizeElementPattern.match(l_str).hasMatch())
    {
        onError();
    }
    int sizePart = -1;
    for (int i = 0; i < 2 || sizePart < 1; ++i) // maximum two passes: sizePart candidate is replaced to default
    {                                           // during the first pass if there's an error
        sizePart = check();
    }

    Q_ASSERT(sizePart > 0);

    return sizePart;
}


} // namespace
