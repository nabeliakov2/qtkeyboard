#ifndef KEYBOARDLAUNCHER_H
#define KEYBOARDLAUNCHER_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtCore/QSize>
#include <QtCore/QSharedPointer>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QCoreApplication>

#include <QtCore/QDebug>

#include <memory>

#include "core/utilities/keyboardutility.h"

namespace qt_keyboard
{
class AbstractKeyboardBackend;
class Keyboard;

const QByteArray defaultTextCodecName("UTF-8");
const int defOpacity {100};
const int defOutputScreen {0};
const int defKbdStatePollInterval {200};


enum class EventHandlerForX11Type {None, XRecord, Querying};


struct ApplicationConfig
{
    QString qssPath;
    QString cleanupCommand;
    QString keyset;
    QString defaultLayout;  // takes precedence over defaultLayoutIndex
    int defaultLayoutIndex {-1};
    QString layoutType {"common-no-shift"};
    bool draggable {true};
    QVector<QString> layouts;
    bool onTop {true};
    int opacity {defOpacity};
    int outputScreen {defOutputScreen};
    QSize size {100, 20};
    bool relativeWidth {true};
    bool relativeHeight {true};
    bool useTextureSizeIfPossible {true};
    bool hideCursor {false};
    bool isAnimated {true};
    int animationDuration {1000}; // ms
    int rowSpacing {1};
    int buttonSpacing {1};
    bool useX11ShiftBugWorkaround {true};
    int toolTipVOffset {-100};
    QSharedPointer<const LayoutNameMapper> pLayoutNameMapper;
    EventHandlerForX11Type eventHandlerForX11Type {EventHandlerForX11Type::XRecord};
    int kbdStatePollInterval {defKbdStatePollInterval}; // ms
    QString displayNameX11;
};

inline bool checkFile(const QFileInfo &_info)
{
    return _info.exists() && _info.isReadable() && _info.size() && _info.isFile();
}

inline bool checkFile(const QString &_path)
{
    return checkFile(QFileInfo(_path));
}

inline QString makePathAbsolute(const QString &_path)
{
    return QDir::cleanPath(QCoreApplication::applicationDirPath() + "/" + _path);
}

class KeyboardLauncher Q_DECL_FINAL : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(KeyboardLauncher)

public:
    explicit KeyboardLauncher(const QSharedPointer<const ApplicationConfig> &_pConfig,
                              QObject *_parent = Q_NULLPTR);
    virtual ~KeyboardLauncher() Q_DECL_OVERRIDE Q_DECL_FINAL;

    AbstractKeyboardBackend *keyboardBackend() const;
    Keyboard *keyboard() const;

    void run();

private:
    struct LauncherPrivate;
    const std::unique_ptr<LauncherPrivate> pLauncherPrivate;



    void runAsync();
};

} // qt_keyboard

#endif // KEYBOARDLAUNCHER_H



