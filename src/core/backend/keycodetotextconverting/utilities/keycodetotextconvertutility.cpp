#include "core/backend/keycodetotextconverting/utilities/keycodetotextconvertutility.h"

#include <QtCore/QString>

namespace qt_keyboard
{

QString &capsTransform(QString &_text, const bool _shift, const bool _caps)
{
    if (_caps)
    {
        if (_shift) _text = _text.toLower();
        else _text = _text.toUpper();
    }
    return _text;
}

} // qt_keyboard
