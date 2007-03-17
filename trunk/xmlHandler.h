#include <QtXml>
#include <QString>
#include "xmlElement.h"

class xmlHandler : public QXmlDefaultHandler
{
public:
	xmlHandler();
	~xmlHandler();

	bool startElement(const QString &namespaceURI, const QString &localName,
			  const QString &qName, const QXmlAttributes &attributes);
	bool endElement(const QString &namespaceURI, const QString &localName,
			const QString &qName);
	bool characters(const QString &str);
	bool fatalError(const QXmlParseException &exception);
	xmlElement elem;
	QList<xmlElement> xmlElementsList;
	//QString errorString() const;

};
