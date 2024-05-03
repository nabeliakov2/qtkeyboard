#ifndef KEYCODETOTEXTCONVERTUTILITY_H
#define KEYCODETOTEXTCONVERTUTILITY_H

#include <QtCore/QHash>
#include <QtCore/QVector>

class QString;

namespace qt_keyboard
{

using MappingType = QHash<int, QString>;
using LayoutsType = QVector<MappingType>;

QString &capsTransform(QString &_text, const bool _shift, const bool _caps);


} // qt_keyboard

#endif // KEYCODETOTEXTCONVERTUTILITY_H
