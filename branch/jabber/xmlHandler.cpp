#include "xmlHandler.h"
#include <QtXml>

XmlHandler::XmlHandler()
{
	depht = 0;
}

XmlHandler::~XmlHandler()
{

}

bool XmlHandler::startElement(const QString &namespaceURI, const QString &localName,
			  const QString &qName, const QXmlAttributes &attributes)
{
	tEvent newEvent;
	newEvent.name = qName;
	newEvent.openingTag = true;
	events.append(newEvent);
	depht++;
	return true;
}

bool XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
			const QString &qName)
{
	tEvent newEvent;
	newEvent.name = qName;
	newEvent.openingTag = false;
	events.append(newEvent);
	depht--;
	//domNode.setNodeValue(xmlTree);

	if (depht == 0)
		error = true;
	
	return true;
}

bool XmlHandler::characters(const QString &str)
{
	events.last().text = str;
	return true;
}

bool XmlHandler::fatalError(const QXmlParseException &exception)
{
	printf(" ! Fatal error : %s.\n", exception.message().toLatin1().constData());
	return true;
}

QList<XmlHandler::tEvent> XmlHandler::getEvents()
{
	QList<XmlHandler::tEvent> returnEvents = events;
	events.clear();
	return returnEvents;
}

