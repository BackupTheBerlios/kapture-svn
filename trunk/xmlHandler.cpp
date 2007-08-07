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
	Event newEvent;
	newEvent.name = qName;
	newEvent.openingTag = true;
	for (int i = 0; i < attributes.count(); i++)
	{
		newEvent.attributes.append(attributes.qName(i),
					   attributes.uri(i),
					   attributes.localName(i),
					   attributes.value(i));
	}

	e.append(newEvent);
	depht++;
	return true;
}

bool XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
			const QString &qName)
{
	Event newEvent;
	newEvent.name = qName;
	newEvent.openingTag = false;
	newEvent.node = node;
	e.append(newEvent);
	depht--;

	if (depht == 0)
		error = true;
	
	return true;
}

bool XmlHandler::characters(const QString &str)
{
	e.last().text = str;
	return true;
}

bool XmlHandler::fatalError(const QXmlParseException &exception)
{
	printf(" ! Fatal error : %s.\n", exception.message().toLatin1().constData());
	return true;
}

QList<XmlHandler::Event> XmlHandler::events()
{
	QList<XmlHandler::Event> returnEvents = e;
	e.clear();
	return returnEvents;
}
void XmlHandler::setData(QByteArray &dat)
{
	node = dat;
}

