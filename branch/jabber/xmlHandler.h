#include <QtXml>
#include <QString>
#include "stanza.h"

#ifndef XMLHANDLER_H
#define XMLHANDLER_H

/*
 * This class only manage XML stuff.
 * No signals, stanzas,... here.
 */

class XmlHandler : public QXmlDefaultHandler
{
public:
	XmlHandler();
	~XmlHandler();

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
	//QDomDocument getQDomDocument();
	QDomNode getQDomNode();
	QDomNode errorNode;
	QString getStartElementTag();
	bool hasStreamTag;
	bool hasFeaturesTag;
	bool hasProceedTag;
	bool hasStarttlsTag;
	bool tlsRequired;
	bool error;
	QList<QDomElement> elements;
	QDomElement elem;
	QDomDocument getDocument();
	/*struct tEvent 
	{
		char *name;
	};*/
	/*
	 * Replace tEvent by a QDomNode or QDomSomething.
	 */
	//QList<tEvent> events;

private:
	QList<QString> tags; // List al tags received in the stream (check that the stream is valid)
	QString xmlTree; // The received Stream
	QDomNode domNode; // Used to handle the stream
	QString err; 
	int errLine;
	int depht; //depht of the xml stream;
	bool waitForMechanism;
	QString startElementTag;
	QDomDocument doc;
	
};

#endif //XMLHANDLER_H
