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
	/*
	 * It seems that it *MUST* use a QDomDocument...
	 * Only 1 root element !
	 */
	elem = doc.createElement(qName);
	doc.appendChild(elem);
	for(int i = 0; i < attributes.count(); i++)
	{
		elem.setAttribute(attributes.qName(i), attributes.value(i));
	}
	//elements.append(elem);
	printf(" * %s\n", doc.toString().toLatin1().constData());
	depht++;
	/*if (qName == QString("stream:stream"))
		hasStreamTag = true;
	
	if (qName == QString("stream:features"))
		hasFeaturesTag = true;
	
	if (qName == QString("proceed"))
		hasProceedTag = true;
	
	if (qName == QString("starttls"))
		hasStarttlsTag = true;
	if (hasStarttlsTag && qName == QString("required"))
		tlsRequired = true;
	
	if (qName == QString("failure"))
		error = true;
	*/


	if(xmlTree.length() == 0)
	{
		printf(" * WARNING : DATA NOT SET !!!!\n");
		return false;
	}

	return true;
}

bool XmlHandler::endElement(const QString &namespaceURI, const QString &localName,
			const QString &qName)
{
	depht--;
	domNode.setNodeValue(xmlTree);

	if (depht == 0)
		error = true;
	
	return true;
}

bool XmlHandler::characters(const QString &str)
{
	QDomText textNode;
	textNode.setData(str);
	elem.appendChild(textNode);
	//elements.last().appendChild(textNode);
	return true;
}

bool XmlHandler::fatalError(const QXmlParseException &exception)
{
	printf(" ! Fatal error : %s.\n", exception.message().toLatin1().constData());
	return true;
}

void XmlHandler::setData(QByteArray data)
{
	xmlTree = QString(data.constData());
	hasStreamTag = false;
	hasFeaturesTag = false;
	hasProceedTag = false;
}

QDomNode XmlHandler::getQDomNode()
{
	return domNode;
}

QDomDocument XmlHandler::getDocument()
{
	QDomDocument returnDoc = doc;
	doc.clear();
	return returnDoc;
}

QString XmlHandler::getStartElementTag()
{
	return startElementTag;
}

/*QList<QDomElement> XmlHandler::results()
{
	return elements;
}
*/

