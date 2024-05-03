#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QtWidgets/QDialog>

#include <memory>

namespace qt_keyboard
{

class AbstractKeyboardBackend;
class KeyboardKeySet;

struct KeyboardConfig
{
    int rowSpacing {1}; // const
    int buttonSpacing {1}; // const
    bool isDraggable {true};
    bool isX11ShiftBugWorkaroundOn {true};
    QString startingLayout; // const
    int startingLayoutIndex {-1}; // const
    int toolTipVOffset {-50};
};

class Keyboard Q_DECL_FINAL: public QWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(Keyboard)

public:
    Keyboard(AbstractKeyboardBackend *const _pKeyboardBackend,
             const QSharedPointer<const KeyboardKeySet> &_pKeySet,
             QWidget *_parent = Q_NULLPTR);
    Keyboard(AbstractKeyboardBackend *const _pKeyboardBackend,
             const QSharedPointer<const KeyboardKeySet> &_pKeySet,
             const QSharedPointer<const KeyboardConfig> &_pConfig,
             QWidget *_parent = Q_NULLPTR);
    virtual ~Keyboard() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL;

    const KeyboardKeySet *keyboardKeySet() const;
    AbstractKeyboardBackend *keyboardBackend() const;


signals:
    void closed() const;

protected:
/// From QDialog
/// reimplented
    virtual void showEvent(QShowEvent *_event) Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void closeEvent(QCloseEvent *_event) Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void mouseMoveEvent(QMouseEvent *_event) Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void mousePressEvent(QMouseEvent *_event) Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual void mouseReleaseEvent(QMouseEvent *_event) Q_DECL_OVERRIDE Q_DECL_FINAL;


private:
    struct KeyboardPrivate;
    const std::unique_ptr<KeyboardPrivate> pKeyboardPrivate;

    void init();
    void on_button_pressed();
    void on_button_released();




};

} // qt_keyboard

#endif // KEYBOARD_H
