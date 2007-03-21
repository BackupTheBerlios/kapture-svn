#include <QtXml>
#include <QString>
#include "stanza.h"

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
	// MUST be called to set the stream data and to allow the creation of xmlTree.
	void setData(QByteArray data);
	stanza s;
	QString nameSpace; // Not used but will certainly be....
private:
	QList<QString> tags; // List al tags received in the stream (check that the stream is valid)
	QString xmlTree; // The received Stream
	QDomDocument doc; // Used to handle the stream
	QString err; 
	int errLine;
	int o; //opened tags
	int c; //closed tags
	
};
