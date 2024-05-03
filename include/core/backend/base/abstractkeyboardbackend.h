#ifndef ABSTRACTKEYBOARDBACKEND_H
#define ABSTRACTKEYBOARDBACKEND_H

#include <QtCore/QObject>

#include <memory>

#include "core/utilities/keyboardutility.h"
#include "core/backend/keycodetotextconverting/keycodetotextconvertstrategyinterface.h"

class QWidget;

namespace qt_keyboard
{

using LayoutNameMapper = QHash<QString, QString>;

class AbstractKeyboardBackend : public QObject,
                                public KeycodeToTextConvertStrategyInterface
{
    Q_OBJECT
    Q_DISABLE_COPY(AbstractKeyboardBackend)

public:
    explicit AbstractKeyboardBackend(QObject *_parent = Q_NULLPTR);
    virtual ~AbstractKeyboardBackend() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE;

    void setTextCached(const int _set);
    bool isTextCached() const;
    void setForcedKeycodeToText(const QSharedPointer<const KeycodeToTextConvertStrategyInterface> &_pForcedKeyText);
    const KeycodeToTextConvertStrategyInterface *forcedKeycodeToText() const;
    void unsetForcedKeycodeToText();
    void setLayoutNameMapper(const QSharedPointer<const LayoutNameMapper> &_pLayoutMapper);
    const LayoutNameMapper *layoutNameMapper() const;
    void unsetLayoutNameMapper();



    QString text(const int _keycode) const;
    bool isInitialized() const Q_DECL_NOEXCEPT;
    QStringList layouts() const;
    QString layout(const int i) const;
    QString currentLayout() const;
    int layoutCount() const;
    QSet<int> pressedKeys() const;
    bool isKeyPressed(const int _keycode) const;
    int currentLayoutIndex() const Q_DECL_NOEXCEPT;
    KeyboardUtility::LocksStateFlags locksState() const Q_DECL_NOEXCEPT;
    bool isCapsLockOn() const Q_DECL_NOEXCEPT;
    bool isNumLockOn() const Q_DECL_NOEXCEPT;
    bool isScrollLockOn() const Q_DECL_NOEXCEPT;
    void switchToNextLayout();
    void switchToLayout(const int _layoutIndex);

    virtual void findAloneWindow() const = 0;
    virtual void configureMainWidget(QWidget *const _pWidget) const = 0;
    virtual void simulateKeyEvent(const int _keycode, const bool _pressed) const = 0;


/// From KeycodeToTextConvertStrategyInterface
/// For reference
/// Returns backend-specific text for keycode
    virtual QString keyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false, const bool _caps = false) const = 0;
    virtual QString keyTextChecked(bool &_hasKeyTextReturn, const int _keycode, const int _layoutIndex,
                                   const bool _shift = false, const bool _caps = false) const = 0;
    virtual bool hasKeyText(const int _keycode, const int _layoutIndex,
                            const bool _shift = false) const = 0;


signals:
    void initialized() const;
    void layoutsChanged(const QStringList&) const; // _newLayouts
    void currentLayoutChanged(const int) const; // _newCurrentLayout
    void currentLayoutUpdated();
    void lockStateChanged(const KeyboardUtility::LocksStateFlags&,
                          const KeyboardUtility::LocksStateFlags&) const; // _newLockState, _prevLockState
    void keysUpdated(const int, const bool) const; // _keycode, _pressed

    void termRequested() const;


protected:
    void updateInitialized();
    void updateLayouts(const QStringList &_newLayouts);
    void updateCurrentLayout(const int _newCurrentLayout);
    void updateLockState(const KeyboardUtility::LocksStateFlags &_newLockState);
    void updateKeyState(const int _keycode, const bool _pressed);

    virtual void doSwitchToLayout(const int _layoutIndex) = 0;

private:
    struct BackendPrivate;
    const std::unique_ptr<BackendPrivate> pBackendPrivate;

};


} // qt_keyboard

#endif // ABSTRACTKEYBOARDBACKEND_H
