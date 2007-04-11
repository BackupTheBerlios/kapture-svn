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
	stanza s;
	QString nameSpace; // Not used but will certainly be....
	bool error;
	struct tEvent 
	{
		QString name;
		QString text;
		bool openingTag;
	};
	QList<tEvent> events;
	QList<tEvent> getEvents();

private:
	QString err; 
	int errLine;
	int depht; //depht of the xml stream;
	
};

#endif //XMLHANDLER_H
