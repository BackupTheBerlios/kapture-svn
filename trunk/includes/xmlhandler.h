#include <QtXml>
#include <QString>
//#include "stanza.h"
#include "event.h"

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
	//stanza s;
	QString nameSpace; // Not used but will certainly be....
	void setData(QByteArray &data);
	bool error;

	Event* read();

private:
	QString err; 
	int errLine;
	int depth; //depth of the xml stream;
	QList<Event*> eventList; //events list.

	QDomDocument doc;
	QDomElement current;
	QDomElement root;
};

#endif //XMLHANDLER_H
