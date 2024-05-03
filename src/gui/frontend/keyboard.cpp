#include "gui/frontend/keyboard.h"

#include <QtCore/QPointer>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include <QtCore/QTimer>

#include <QtGui/QShowEvent>

#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QToolTip>

#include "core/backend/base/abstractkeyboardbackend.h"
#include "core/backend/keysets/keyboardkeyset.h"
#include "core/utilities/keyboardutility.h"



// TODO: needs huge optimizations in button text updating
// Now all text on all buttons is always updated (by backend querying)
namespace qt_keyboard
{

namespace
{
void setButtonObjectDown(QObject *const _pObject, const bool _down);

}

using ButtonForKeycodeMapType = QHash<int, QAbstractButton*>;
using KeycodeForButtonObjectMapType = QHash<const QObject*, int>;

struct Keyboard::KeyboardPrivate Q_DECL_FINAL // private
{
// properties
    const QPointer<AbstractKeyboardBackend> pKeyboardBackend;
    const QSharedPointer<const KeyboardConfig> pKeyboardConfig;
    const QSharedPointer<const KeyboardKeySet> pKeyboardKeySet;
    //bool b_isDraggable {true};

// state
    ButtonForKeycodeMapType buttonForKeycodeContainer;
    KeycodeForButtonObjectMapType keycodeForButtonObjContainer;
    QSet<int> pressedModifiers;
    bool b_needsUpdateOnShow {false};
    QPoint mouseDragPoint;
    bool b_isBeingDragged {false};
    bool b_isMouseDragPointSet {false};

    KeyboardPrivate(AbstractKeyboardBackend *const _pKeyboardBackend,
                    const QSharedPointer<const KeyboardKeySet> _pKeyboardKeySet)
        : pKeyboardBackend(_pKeyboardBackend),
          pKeyboardConfig(new KeyboardConfig),
          pKeyboardKeySet(_pKeyboardKeySet)
    {}
    KeyboardPrivate(AbstractKeyboardBackend *const _pKeyboardBackend,
                    const QSharedPointer<const KeyboardConfig> &_pKeyBoardConfig,
                    const QSharedPointer<const KeyboardKeySet> _pKeyboardKeySet)
        : pKeyboardBackend(_pKeyboardBackend),
          pKeyboardConfig(_pKeyBoardConfig),
          pKeyboardKeySet(_pKeyboardKeySet)
    {}
    ~KeyboardPrivate() Q_DECL_NOEXCEPT = default;

    QAbstractButton *createButton(QWidget *const _parent = Q_NULLPTR) const;
    int processButtonOperation(const QObject *const _pButtonObject, const bool _pressed) const;
    void updateCurrentLayout(const bool _isVisible = true);

private:
    KeyboardPrivate() = delete;
    KeyboardPrivate(const KeyboardPrivate &_other) = delete;
    KeyboardPrivate(KeyboardPrivate &&_other) = delete;

    KeyboardPrivate &operator=(const KeyboardPrivate &_other) = delete;
    KeyboardPrivate &operator=(KeyboardPrivate &&_other) = delete;
};

Keyboard::Keyboard(AbstractKeyboardBackend *const _pKeyboardBackend,
                   const QSharedPointer<const KeyboardKeySet> &_pKeySet,
                   QWidget *_parent /* = Q_NULLPTR */)  // public
    : QWidget(_parent),
      pKeyboardPrivate(new KeyboardPrivate(_pKeyboardBackend, _pKeySet))
{
    init();
}

Keyboard::Keyboard(AbstractKeyboardBackend *const _pKeyboardBackend,
                   const QSharedPointer<const KeyboardKeySet> &_pKeySet,
                   const QSharedPointer<const KeyboardConfig> &_pConfig,
                   QWidget *_parent /* = Q_NULLPTR */) // public
    : QWidget(_parent),
      pKeyboardPrivate(_pConfig ? new KeyboardPrivate(_pKeyboardBackend, _pConfig, _pKeySet) :
                                  new KeyboardPrivate(_pKeyboardBackend, _pKeySet))
{
    if (!_pKeySet)
    {
        throw std::invalid_argument(std::string("Keyset must be provided in ") + Q_FUNC_INFO);
    }

    init();
}

Keyboard::~Keyboard() Q_DECL_NOEXCEPT   // virtual public override final
{}

const KeyboardKeySet *Keyboard::keyboardKeySet() const  // private
{
    Q_ASSERT(pKeyboardPrivate);
    return pKeyboardPrivate -> pKeyboardKeySet.data();
}

AbstractKeyboardBackend *Keyboard::keyboardBackend() const  // private
{
    Q_ASSERT(pKeyboardPrivate);

    return pKeyboardPrivate -> pKeyboardBackend.data();
}

void Keyboard::showEvent(QShowEvent *_event) // virtual public override final
{
    if (!_event -> spontaneous())
    {
        Q_ASSERT(pKeyboardPrivate && pKeyboardPrivate -> pKeyboardBackend);

        if (pKeyboardPrivate -> b_needsUpdateOnShow &&
                pKeyboardPrivate -> pKeyboardBackend -> isInitialized())
        {
            pKeyboardPrivate -> updateCurrentLayout();
        }
    }
    QWidget::showEvent(_event);
}

void Keyboard::closeEvent(QCloseEvent *_event) // virtual public override final
{
    Q_ASSERT(pKeyboardPrivate && pKeyboardPrivate -> pKeyboardBackend);

    if (!pKeyboardPrivate -> pressedModifiers.isEmpty())
    {
        foreach (const int keycode, pKeyboardPrivate -> pressedModifiers)
        {
            if (keycode > -1 && KeyboardUtility::ModifierSet.contains(keycode))
            {
                pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(keycode, false);
                QCoreApplication::processEvents(QEventLoop::AllEvents, KeyboardUtility::processEventsMaxTime);
            }
            else
            {
                qWarning() << "Invalid modifier keycode:" << keycode;
            }

        }
    }
    QWidget::closeEvent(_event);
    emit closed();
}

void Keyboard::mousePressEvent(QMouseEvent *_event) // virtual public override final
{
    Q_ASSERT(pKeyboardPrivate && pKeyboardPrivate -> pKeyboardConfig);

    if (pKeyboardPrivate -> pKeyboardConfig -> isDraggable)
    {
        if (_event -> buttons() == Qt::LeftButton)
        {
            QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        }
    }
    QWidget::mousePressEvent(_event);
}

void Keyboard::mouseReleaseEvent(QMouseEvent*) // virtual public override final
{
    Q_ASSERT(pKeyboardPrivate);

    QGuiApplication::restoreOverrideCursor();

    pKeyboardPrivate -> b_isBeingDragged = false;
    pKeyboardPrivate -> b_isMouseDragPointSet = false;
}

void Keyboard::mouseMoveEvent(QMouseEvent *_event)  // virtual public override final
{
    Q_ASSERT(pKeyboardPrivate && pKeyboardPrivate -> pKeyboardConfig);

    if (pKeyboardPrivate -> pKeyboardConfig -> isDraggable)
    {
        if (_event -> buttons() == Qt::LeftButton)
        {
            if (!pKeyboardPrivate -> b_isMouseDragPointSet)
            {
                pKeyboardPrivate -> mouseDragPoint = _event -> globalPos();
                pKeyboardPrivate -> b_isMouseDragPointSet = true;
            }
            else
            {
                const QPoint delta(_event -> globalPos() - pKeyboardPrivate -> mouseDragPoint);
                if (!pKeyboardPrivate -> b_isBeingDragged)
                {
                    if (delta.manhattanLength() >= QApplication::startDragDistance())
                    {
                        pKeyboardPrivate -> b_isBeingDragged = true;

                        if (!QGuiApplication::overrideCursor())
                        {
                            QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
                        }
                    }
                }

                if (pKeyboardPrivate -> b_isBeingDragged)
                {
                    move(x() + delta.x(), y() + delta.y());
                    pKeyboardPrivate -> mouseDragPoint = _event -> globalPos();
                }
            }
        }
    }
    QWidget::mouseMoveEvent(_event);
}

void Keyboard::init()   // private
{
    Q_ASSERT(pKeyboardPrivate && pKeyboardPrivate -> pKeyboardBackend &&
             pKeyboardPrivate -> pKeyboardConfig && pKeyboardPrivate -> pKeyboardKeySet);

    if (!connect(pKeyboardPrivate -> pKeyboardBackend, &AbstractKeyboardBackend::termRequested,
                 this, &QWidget::close))
    {
        throw std::runtime_error(std::string("Signal termRequested() connection error in") + Q_FUNC_INFO);
    }

    QVBoxLayout *const keyboardLayout = new QVBoxLayout;
    keyboardLayout -> setSpacing(pKeyboardPrivate -> pKeyboardConfig -> rowSpacing);
    const int rowCount = pKeyboardPrivate -> pKeyboardKeySet -> rowCount();
    for (int row = 0; row < rowCount; ++row)
    {
        QHBoxLayout *const rowLayout = new QHBoxLayout;
        rowLayout -> setSpacing(pKeyboardPrivate -> pKeyboardConfig -> buttonSpacing);
        const int buttonCount = pKeyboardPrivate -> pKeyboardKeySet -> keyCount(row);
        for (int buttonIdx = 0; buttonIdx < buttonCount; ++buttonIdx)
        {
            const int currKeycode = pKeyboardPrivate -> pKeyboardKeySet -> keycodeAt(row, buttonIdx);
            // special cases
            if (currKeycode == KeyboardUtility::SpacerStretch)
            {
                rowLayout -> addStretch();
            }
            else // common button
            {
                QAbstractButton *const currButton = pKeyboardPrivate -> createButton(this);
                if (currKeycode == KeyboardUtility::Space)
                {
                    currButton -> setObjectName("spaceButton");
                }
                else if (currKeycode == KeyboardUtility::LeftShift ||
                         currKeycode == KeyboardUtility::RightShift)
                {
                    currButton -> setObjectName("shiftButton");
                }
                else if (currKeycode == KeyboardUtility::Enter)
                {
                    currButton -> setObjectName("enterButton");
                }

                rowLayout -> addWidget(currButton);
                pKeyboardPrivate -> buttonForKeycodeContainer.insert(currKeycode, currButton);
                pKeyboardPrivate -> keycodeForButtonObjContainer.insert(currButton, currKeycode);
                if (!connect(currButton, &QAbstractButton::pressed,
                             this, &Keyboard::on_button_pressed))
                {
                    throw std::runtime_error(std::string("Signal pressed() connection error in") + Q_FUNC_INFO);
                }
                if (!connect(currButton, &QAbstractButton::released,
                             this, &Keyboard::on_button_released))
                {
                    throw std::runtime_error(std::string("Signal released() connection error in") + Q_FUNC_INFO);
                }
            }
        }
        keyboardLayout -> addLayout(rowLayout);
    }
    setLayout(keyboardLayout);


    if (pKeyboardPrivate -> pKeyboardBackend -> isInitialized())
    {
        pKeyboardPrivate -> updateCurrentLayout(isVisible());
    }

    if (!connect(pKeyboardPrivate -> pKeyboardBackend.data(), &AbstractKeyboardBackend::initialized, this,
                 [this]()
                 {
                     Q_ASSERT(pKeyboardPrivate && pKeyboardPrivate -> pKeyboardConfig);

                     if (pKeyboardPrivate -> pKeyboardConfig -> isX11ShiftBugWorkaroundOn)
                     {
                         //pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(KeyboardUtility::LeftCtrl, true);
                         pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(KeyboardUtility::LeftShift, false);
                         QCoreApplication::processEvents(QEventLoop::AllEvents, KeyboardUtility::processEventsMaxTime);
                     }

                     const QString &startingLayout(pKeyboardPrivate -> pKeyboardConfig -> startingLayout);
                     int layoutIndex = -1;
                     const QStringList &layoutList(pKeyboardPrivate -> pKeyboardBackend -> layouts());
                     const int llsize = layoutList.size();
                     if (!startingLayout.isEmpty())
                     {
                        Q_ASSERT(pKeyboardPrivate -> pKeyboardBackend);

                        if (!layoutList.isEmpty())
                        {
                            for (int i = 0; i < llsize; ++i)
                            {
                                if (!QString::compare(layoutList.at(i), startingLayout, Qt::CaseInsensitive))
                                {
                                    layoutIndex = i;
                                    break;
                                }   // -> if (!QString::compare(layoutList.at(i), startingLayout, Qt::CaseInsensitive))
                            }   // -> for (int i = 0; i < llsize; ++i)
                        }   // -> if (!layoutList.isEmpty())
                        if (layoutIndex < 0)
                        {
                            qWarning() << "Starting layout" << startingLayout << "not found; trying to use index";
                        }
                     }  // -> if (!startingLayout.isEmpty())
                     if (layoutIndex < 0)
                     {
                        if (pKeyboardPrivate -> pKeyboardConfig -> startingLayoutIndex > -1)
                        {
                            if (pKeyboardPrivate -> pKeyboardConfig -> startingLayoutIndex < llsize)
                            {
                                layoutIndex = pKeyboardPrivate -> pKeyboardConfig -> startingLayoutIndex;
                            }
                            else
                            {
                                qWarning() << "Invalid starting layout index" << pKeyboardPrivate -> pKeyboardConfig -> startingLayoutIndex;
                            }
                        }
                     }
                     if (layoutIndex > -1)
                     {
                         pKeyboardPrivate -> pKeyboardBackend -> switchToLayout(layoutIndex);
                     }
                 })
        )
    {
        throw std::runtime_error(std::string("Signal layoutsChanged() connection error in") + Q_FUNC_INFO);
    }

    if (!connect(pKeyboardPrivate -> pKeyboardBackend.data(), &AbstractKeyboardBackend::layoutsChanged, this,
                 [this](const QStringList&)
                 {
                    pKeyboardPrivate -> updateCurrentLayout(this -> isVisible());
                 })
        )
    {
        throw std::runtime_error(std::string("Signal layoutsChanged() connection error in") + Q_FUNC_INFO);
    }
    if (!connect(pKeyboardPrivate -> pKeyboardBackend.data(), &AbstractKeyboardBackend::currentLayoutChanged, this,
                 [this](const int)
                 {

                    pKeyboardPrivate -> updateCurrentLayout(this -> isVisible());
                 })
        )
    {
        throw std::runtime_error(std::string("Signal currentLayoutChanged() connection error in") + Q_FUNC_INFO);
    }
    if (!connect(pKeyboardPrivate -> pKeyboardBackend.data(), &AbstractKeyboardBackend::currentLayoutUpdated, this,
                 [this]()
                 {
                    pKeyboardPrivate -> updateCurrentLayout(this -> isVisible());
                 })
        )
    {
        throw std::runtime_error(std::string("Signal currentLayoutUpdated() connection error in") + Q_FUNC_INFO);
    }
    if (!connect(pKeyboardPrivate -> pKeyboardBackend.data(), &AbstractKeyboardBackend::lockStateChanged, this,
                 [this](const KeyboardUtility::LocksStateFlags &_newLockState,
                        const KeyboardUtility::LocksStateFlags &_prevLockState)
                 {
                    const KeyboardUtility::LocksStateFlags lockXOR = _newLockState ^ _prevLockState;
                    if (lockXOR & KeyboardUtility::CapsLockFlag)
                    {
                        Q_ASSERT(pKeyboardPrivate);

                        QAbstractButton *const capsLockButton = pKeyboardPrivate -> buttonForKeycodeContainer.value(KeyboardUtility::CapsLock, Q_NULLPTR);
                        if (capsLockButton)
                        {
                            capsLockButton -> setDown(_newLockState & KeyboardUtility::CapsLockFlag);
                        }
                        pKeyboardPrivate -> updateCurrentLayout(this -> isVisible());

                    }
                 })
        )
    {
        throw std::runtime_error(std::string("Signal lockStateChanged() connection error in") + Q_FUNC_INFO);
    }
    if (!connect(pKeyboardPrivate -> pKeyboardBackend.data(), &AbstractKeyboardBackend::keysUpdated, this,
                 [this](const int _keycode, const bool _pressed)
                 {
                     Q_ASSERT(pKeyboardPrivate);

                     if (KeyboardUtility::ModifierSet.contains(_keycode))
                     {
                        if (pKeyboardPrivate -> pressedModifiers.contains(_keycode))
                        {
                             if (!_pressed)
                             {
                                pKeyboardPrivate -> pressedModifiers.remove(_keycode);
                                pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(_keycode, false);
                                QAbstractButton *const releasedButton = pKeyboardPrivate -> buttonForKeycodeContainer.value(_keycode, Q_NULLPTR);
                                if (releasedButton)
                                {
                                    releasedButton -> setDown(false);
                                }
                                if (_keycode == KeyboardUtility::LeftShift ||
                                    _keycode == KeyboardUtility::RightShift)
                                {
                                    pKeyboardPrivate -> updateCurrentLayout(this -> isVisible());
                                }

                                QCoreApplication::processEvents(QEventLoop::AllEvents, KeyboardUtility::processEventsMaxTime);
                                return;

                             }
                        }
                     }
                     if (_keycode != KeyboardUtility::CapsLock)
                     {
                         if (KeyboardUtility::ModifierSet.contains(_keycode))
                         {
                             if (_keycode == KeyboardUtility::LeftShift ||
                                                  _keycode == KeyboardUtility::RightShift)
                             {

                                pKeyboardPrivate -> updateCurrentLayout(this -> isVisible());
                             }
                         }
                         else
                         {
                             if (!pKeyboardPrivate -> pressedModifiers.isEmpty())
                             {
                                 foreach (const int keycode, pKeyboardPrivate -> pressedModifiers)
                                 {
                                     if (keycode > -1 && KeyboardUtility::ModifierSet.contains(keycode))
                                     {
                                         pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(keycode, false);
                                         QCoreApplication::processEvents(QEventLoop::AllEvents, KeyboardUtility::processEventsMaxTime);
                                     }
                                     else
                                     {
                                         qWarning() << "Invalid modifier keycode:" << keycode;
                                     }

                                 }
                             }
                             if (!pKeyboardPrivate -> pressedModifiers.isEmpty())
                             {
                                foreach (const int keycode, pKeyboardPrivate -> pressedModifiers)
                                {
                                    if (keycode > -1 && KeyboardUtility::ModifierSet.contains(keycode))
                                    {
                                        pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(keycode, false);
                                        QCoreApplication::processEvents(QEventLoop::AllEvents, KeyboardUtility::processEventsMaxTime);
                                    }
                                    else
                                    {
                                        qWarning() << "Invalid modifier keycode:" << keycode;
                                    }
                                }
                             }
                         }
                         QAbstractButton *const pButton = pKeyboardPrivate -> buttonForKeycodeContainer.value(_keycode, Q_NULLPTR);
                         if (pButton)
                         {
                            pButton -> setDown(_pressed);
                         }
                    }
                })
        )
    {
        throw std::runtime_error(std::string("Signal keysUpdated() connection error in") + Q_FUNC_INFO);
    }


}

void Keyboard::on_button_pressed()  // private (slot) TODO: noexcept
{
    Q_ASSERT(pKeyboardPrivate && pKeyboardPrivate -> pKeyboardBackend);

    QObject *const pSenderObject = sender();

    const int keycode = pKeyboardPrivate -> processButtonOperation(pSenderObject, true);
    if (keycode > -1)
    {
        if (keycode < KeyboardUtility::UserKeys)
        {
            if (!pKeyboardPrivate -> pressedModifiers.contains(keycode))
            {


                {
                    pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(keycode, true);
                    QCoreApplication::processEvents(QEventLoop::AllEvents, KeyboardUtility::processEventsMaxTime);
                }
            }
        }

        QAbstractButton *const pButton = qobject_cast<QAbstractButton*>(pSenderObject);
        if (pButton)
        {
            if (!pButton -> text().isEmpty())
            {
                QToolTip::showText(QCursor::pos() +
                                   QPoint(0, pKeyboardPrivate -> pKeyboardConfig -> toolTipVOffset),
                                   pButton -> text(), pButton, pButton -> rect());
            }
        }


    }
}

void Keyboard::on_button_released() // private (slot) TODO: noexcept (slot)
{
    Q_ASSERT(pKeyboardPrivate && pKeyboardPrivate -> pKeyboardBackend);

    QObject *const pSenderObject = sender();

    Q_ASSERT(pSenderObject);

    const int keycode = pKeyboardPrivate -> processButtonOperation(pSenderObject, false);
    if (keycode > -1)
    {

        if (keycode < KeyboardUtility::UserKeys)
        {
            // special cases
            if (KeyboardUtility::ModifierSet.contains(keycode))
            {
                if (pKeyboardPrivate -> pressedModifiers.contains(keycode))
                {
                    pKeyboardPrivate -> pressedModifiers.remove(keycode);
                    pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(keycode, false);
                }
                else
                {
                    pKeyboardPrivate -> pressedModifiers.insert(keycode);
                    setButtonObjectDown(pSenderObject, true);
                }
            }
            else
            {
                pKeyboardPrivate -> pKeyboardBackend -> simulateKeyEvent(keycode, false);
                if (keycode == KeyboardUtility::CapsLock)
                {
                    setButtonObjectDown(pSenderObject, pKeyboardPrivate -> pKeyboardBackend -> isCapsLockOn());
                }
            }
        } // special keys
        else if (keycode == KeyboardUtility::SwitchLayout)
        {
            pKeyboardPrivate -> pKeyboardBackend -> switchToNextLayout();
        }
        else if (keycode == KeyboardUtility::Close)
        {
            close();
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////
/// Keyboard::KeyboardPrivate
QAbstractButton *Keyboard::KeyboardPrivate::createButton(QWidget *const _parent /* = Q_NULLPTR */) const  // private::public
{
    QPushButton *const button = new QPushButton(_parent);
    // minimum size
    // icon
    return button;
}

int Keyboard::KeyboardPrivate::processButtonOperation(const QObject *const _pButtonObject, const bool) const   // private::public
{
    Q_ASSERT(_pButtonObject);

    if (keycodeForButtonObjContainer.contains(_pButtonObject))
    {
        const int keycode = keycodeForButtonObjContainer.value(_pButtonObject);
        if (keycode < 0)
        {
            qWarning() << "Invalid keycode:" << keycode;
        }
        else
        {
            return keycode;
        }

    }
    else
    {
        qWarning() << "No keycode found for sender" << _pButtonObject;
    }
    return -1;
}

void Keyboard::KeyboardPrivate::updateCurrentLayout(const bool _isVisible) // private::public
{
    Q_ASSERT(pKeyboardBackend);

    if (!_isVisible)
    {
        b_needsUpdateOnShow = true;
        return;
    }

    for (auto buttonIterator = buttonForKeycodeContainer.constBegin();
         buttonIterator != buttonForKeycodeContainer.constEnd(); ++buttonIterator)
    {
        const int currKeycode = buttonIterator.key();
        const QPointer<QAbstractButton> pCurrButton = buttonIterator.value();

        Q_ASSERT(pCurrButton);

        const QString currText(pKeyboardBackend -> text(currKeycode));
        if (!currText.isEmpty())
        {
            pCurrButton -> setText(currText);
        }
    }

    QAbstractButton *const pSwitchLayoutButton = buttonForKeycodeContainer.value(KeyboardUtility::SwitchLayout, Q_NULLPTR);
    if (pSwitchLayoutButton)
    {
        pSwitchLayoutButton -> setText(pKeyboardBackend -> currentLayout());
    }

}



namespace
{
void setButtonObjectDown(QObject *const _pObject, const bool _down)
{
    QAbstractButton *const pButton = qobject_cast<QAbstractButton*>(_pObject);
    if (pButton)
    {
        pButton -> setDown(_down);
    }
    else
    {
        qWarning() << "Object is not a button";
    }
}

}


} // qt_keyboard

