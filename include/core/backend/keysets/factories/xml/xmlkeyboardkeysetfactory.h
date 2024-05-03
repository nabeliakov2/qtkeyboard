#ifndef XMLKEYBOARDKEYSETFACTORY_H
#define XMLKEYBOARDKEYSETFACTORY_H

#include <QtCore/QString>

#include "core/backend/keysets/factories/keyboardkeysetfactoryinterface.h"

namespace qt_keyboard
{

class XMLKeyboardKeySetFactory Q_DECL_FINAL : public KeyboardKeySetFactoryInterface
{
public:
    explicit XMLKeyboardKeySetFactory(const QString &_xmlFilePath);
    virtual ~XMLKeyboardKeySetFactory() Q_DECL_NOEXCEPT Q_DECL_OVERRIDE Q_DECL_FINAL = default;

    XMLKeyboardKeySetFactory(const XMLKeyboardKeySetFactory &_other) = default;
    XMLKeyboardKeySetFactory(XMLKeyboardKeySetFactory &&_other) = default;

    XMLKeyboardKeySetFactory &operator=(const XMLKeyboardKeySetFactory &_other) = default;
    XMLKeyboardKeySetFactory &operator=(XMLKeyboardKeySetFactory &&_other) = default;

    void setXMLFilePath(const QString &_xmlFilePath);
    inline QString xmlFilePath() const Q_DECL_NOEXCEPT
    {
        return s_xmlFilePath;
    }

    virtual KeyboardKeySet *createKeyboardKeySet() const Q_DECL_OVERRIDE Q_DECL_FINAL;

private:
    QString s_xmlFilePath;
};

} // qt_keyboard

#endif // XMLKEYBOARDKEYSETFACTORY_H
