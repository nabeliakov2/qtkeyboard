#include "core/utilities/keyboardutility.h"

#include <QtCore/QSet>

namespace qt_keyboard
{

const QSet<int> KeyboardUtility::ModifierSet(QSet<int>() << KeyboardUtility::LeftShift << KeyboardUtility::RightShift <<
                                             KeyboardUtility::LeftCtrl << KeyboardUtility::RightCtrl <<
                                             KeyboardUtility::LeftAlt << KeyboardUtility::RightAlt); // static public

} // qt_keyboard
