#include "core/backend/keysets/factories/xml/xmlkeyboardkeysetfactory.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QXmlStreamReader>
#include <QtCore/QDebug>

#include "core/backend/keysets/keyboardkeyset.h"
#include "core/utilities/keyboardutility.h"

namespace qt_keyboard
{

namespace
{
const QString keysetHeaderElement("keyset");
const QString typeElement("type");
const QString keysElement("keys");
const QString rowElement("row");
const QString keyElement("key");
const QString spaceElement("space");

const QString versionAttribute("version");
const QString keycodeAttribute("keycode");
const QString typeAttribute("type");
const QString stretchValue("stretch");
const QString switchValue("switch");
const QString closeValue("close");

const QStringList correctVersions {QString("2.0")};

void readKeysetElement(QXmlStreamReader &_reader,
                       KeyboardKeySetConfig *const _pKeyboardLayoutConfig);


} // namespace

XMLKeyboardKeySetFactory::XMLKeyboardKeySetFactory(const QString &_xmlFilePath)   // public explicit
    : KeyboardKeySetFactoryInterface()
{
    setXMLFilePath(_xmlFilePath);
}

void XMLKeyboardKeySetFactory::setXMLFilePath(const QString &_xmlFilePath) // public
{
    if (_xmlFilePath != s_xmlFilePath)
    {
        if (_xmlFilePath.isEmpty())
        {
            throw std::invalid_argument(std::string("Empty XML file path in ") + Q_FUNC_INFO);
        }
        const QFileInfo xmlFileInfo(_xmlFilePath);
        if (!xmlFileInfo.exists() || !xmlFileInfo.isFile() ||
                !xmlFileInfo.isReadable() || !xmlFileInfo.size())
        {
            throw std::runtime_error(std::string("Invalid file in ") + Q_FUNC_INFO);
        }
        s_xmlFilePath = _xmlFilePath;
    }
}

KeyboardKeySet *XMLKeyboardKeySetFactory::createKeyboardKeySet() const // virtual public override final
{
    if (s_xmlFilePath.isEmpty())
    {
        throw std::logic_error(std::string("XML file wasn't specified") + Q_FUNC_INFO);
    }

    QFile xmlFile(s_xmlFilePath);
    if (xmlFile.open(QIODevice::ReadOnly))
    {
        QXmlStreamReader xmlReader(&xmlFile);
        if (xmlReader.readNextStartElement())
        {
            const QStringRef name(xmlReader.name());
            if (!QStringRef::compare(name, keysetHeaderElement))
            {
                QString version;
                const QXmlStreamAttributes attrs(xmlReader.attributes());
                if (!attrs.isEmpty() && attrs.hasAttribute(versionAttribute))
                {
                    version = attrs.value(versionAttribute).toString();
                    if (!correctVersions.contains(version))
                    {
                        xmlReader.raiseError("Invalid layout version");
                    }
                    const QSharedPointer<KeyboardKeySetConfig> lpKeyboardLayoutConfig(new KeyboardKeySetConfig);
                    readKeysetElement(xmlReader, lpKeyboardLayoutConfig.data());

                    return new KeyboardKeySet(lpKeyboardLayoutConfig);
                }
                else
                {
                    xmlReader.raiseError(QString("Unknown data version in %1").arg(Q_FUNC_INFO));
                }
            }
            else
            {
                xmlReader.raiseError("Invalid layout file");
            }
        }
    }
    return nullptr;
}

namespace
{

void readKeysElement(QXmlStreamReader &_reader,
                     KeyboardKeySetConfig *const _pKeyboardLayoutConfig);
void readRowElement(QXmlStreamReader &_reader,
                    KeyboardKeySetConfig *const _pKeyboardLayoutConfig);
void readKeyElement(QXmlStreamReader &_reader,
                    KeyboardKeySetConfig *const _pKeyboardLayoutConfig);
void readSpaceElement(QXmlStreamReader &_reader,
                      KeyboardKeySetConfig *const _pKeyboardLayoutConfig);
void skipElement(QXmlStreamReader &_reader);

void readKeysetElement(QXmlStreamReader &_reader,
                       KeyboardKeySetConfig *const _pKeyboardLayoutConfig)
{
    Q_ASSERT(_pKeyboardLayoutConfig);

    while (_reader.readNextStartElement())
    {
        if (!QStringRef::compare(_reader.name(), typeElement))
        {
            _pKeyboardLayoutConfig -> id = _reader.readElementText(QXmlStreamReader::ErrorOnUnexpectedElement);
        }
        else if (!QStringRef::compare(_reader.name(), keysElement))
        {
            readKeysElement(_reader, _pKeyboardLayoutConfig);
        }
        else
        {
            skipElement(_reader);
        }
    }
}

void readKeysElement(QXmlStreamReader &_reader,
                     KeyboardKeySetConfig *const _pKeyboardLayoutConfig)
{
    Q_ASSERT(_pKeyboardLayoutConfig);

    while (_reader.readNextStartElement())
    {
        if (!QStringRef::compare(_reader.name(), rowElement))
        {
            readRowElement(_reader, _pKeyboardLayoutConfig);
        }
        else
        {
            skipElement(_reader);
        }
    }
}

void readRowElement(QXmlStreamReader &_reader,
                    KeyboardKeySetConfig *const _pKeyboardLayoutConfig)
{
    Q_ASSERT(_pKeyboardLayoutConfig);

    _pKeyboardLayoutConfig -> keyCodeList.append(QList<int>());

    while (_reader.readNextStartElement())
    {
        if (!QStringRef::compare(_reader.name(), keyElement))
        {
            readKeyElement(_reader, _pKeyboardLayoutConfig);
        }
        else if (!QStringRef::compare(_reader.name(), spaceElement))
        {
            readSpaceElement(_reader, _pKeyboardLayoutConfig);
        }
        else
        {
            skipElement(_reader);
        }
    }
}


void readKeyElement(QXmlStreamReader &_reader,
                    KeyboardKeySetConfig *const _pKeyboardLayoutConfig)
{
    Q_ASSERT(_pKeyboardLayoutConfig);

    const QXmlStreamAttributes &attrs(_reader.attributes());
    if (attrs.isEmpty())
    {
        qWarning() << "Empty key";
        return;
    }
    bool s;
    int keycode = -1;
    if (attrs.hasAttribute(keycodeAttribute))
    {
        keycode = attrs.value(keycodeAttribute).toInt(&s);
        if (!s)
        {
            _reader.raiseError("Wrong keycode: " + keycode);
            return;
        }
    }
    else if (attrs.hasAttribute(typeAttribute))
    {
        const QStringRef typeStrRef(attrs.value(typeAttribute));
        if (!typeStrRef.isEmpty())
        {
            if (!QStringRef::compare(typeStrRef, switchValue))
            {
                keycode = KeyboardUtility::SwitchLayout;
            }
            else if (!QStringRef::compare(typeStrRef, closeValue))
            {
                keycode = KeyboardUtility::Close;
            }
        }
    }
    if (keycode < 0)
    {
        _reader.raiseError("No keycode");
    }


    QList<QList<int>> &rowList(_pKeyboardLayoutConfig -> keyCodeList);
    QList<int> &currRow(rowList[rowList.size() - 1]);
    currRow.append(keycode);

    _reader.skipCurrentElement();
}

void readSpaceElement(QXmlStreamReader &_reader,
                      KeyboardKeySetConfig *const _pKeyboardLayoutConfig)
{
    Q_ASSERT(_pKeyboardLayoutConfig);

    QList<QList<int>> &rowList(_pKeyboardLayoutConfig -> keyCodeList);
    QList<int> &currRow(rowList[rowList.size() - 1]);

    const QXmlStreamAttributes &attrs(_reader.attributes());
    if (attrs.isEmpty())
    {
        qWarning() << "Empty spacer";
        return;
    }
    if (attrs.hasAttribute(typeAttribute))
    {
        const QStringRef typeStrRef(attrs.value(typeAttribute));
        if (!QStringRef::compare(typeStrRef, stretchValue, Qt::CaseInsensitive))
        {
            currRow.append(KeyboardUtility::SpacerStretch);
        }
    }
    _reader.skipCurrentElement();
}

void skipElement(QXmlStreamReader &_reader)
{
    qWarning() << "Unknown element" << _reader.name();

    _reader.skipCurrentElement();
}


} // namespace



} // qt_keyboard
