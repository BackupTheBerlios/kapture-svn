#include "xmlHandler.h"

xmlHandler::xmlHandler()
{
	o = 0;
	c = 0;
}

xmlHandler::~xmlHandler()
{

}

bool xmlHandler::startElement(const QString &namespaceURI, const QString &localName,
			  const QString &qName, const QXmlAttributes &attributes)
{
	o++;
	tags.append(qName);
	if (qName == QString("stream:stream"))
		nameSpace = namespaceURI;

	if(xmlTree.length() == 0)
	{
		printf(" * WARNING : DATA NOT SET !!!!\n");
		return false;
	}

	return true;
}

bool xmlHandler::endElement(const QString &namespaceURI, const QString &localName,
			const QString &qName)
{
	c++;
	if (qName == tags.at(0) && (c - o) == 0) // It's the last end element so I can parse a DOM tree.
	{
		if(doc.setContent(xmlTree, &err, &errLine))
		{
			printf(" * Parsing OK (DOM).\n");
			s.setType(doc.documentElement().tagName());
		}
		else
			printf(" ! Error while parsing : %s (line %d)\n.", err.toLatin1().constData(), errLine);
		
		if (!doc.elementById(QString("stream:error")).isNull())
		{
			printf(" ! Error ! (DOM)\n");
			return false;
		}
	}
	return true;
}

bool xmlHandler::characters(const QString &str)
{
	return true;
}

bool xmlHandler::fatalError(const QXmlParseException &exception)
{
	printf(" ! Fatal error : %s.\n", exception.message().toLatin1().constData());
	return true;
}

void xmlHandler::setData(QByteArray data)
{
	o = 0;
	c = 0;
	tags.clear();
	xmlTree = QString(data.constData());
}

