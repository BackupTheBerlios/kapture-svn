#include "xmlHandler.h"
#include "xmlElement.h"

xmlHandler::xmlHandler()
{

}

xmlHandler::~xmlHandler()
{

}

bool xmlHandler::startElement(const QString &namespaceURI, const QString &localName,
			  const QString &qName, const QXmlAttributes &attributes)
{
	printf("namespaceURI : %s\nlocalName : %s\nqName : %s\n", namespaceURI.toLatin1().constData(),
								  localName.toLatin1().constData(),
								  qName.toLatin1().constData());
	printf("Attributes : \n");
	for (int i = 0; i < attributes.count(); i++)
		printf(" * %s = %s\n", attributes.qName(i).toLatin1().constData(),
				       attributes.value(i).toLatin1().constData());
	elem.setTagName(localName);
	elem.setAttributes(attributes);

	xmlElementsList.append(elem);
	return true;
}

bool xmlHandler::endElement(const QString &namespaceURI, const QString &localName,
			const QString &qName)
{
	printf("namespaceURI : %s\nlocalName : %s\nqName : %s\n", namespaceURI.toLatin1().constData(),
								  localName.toLatin1().constData(),
								  qName.toLatin1().constData());
	return true;
}

bool xmlHandler::characters(const QString &str)
{
	printf("str : %s\n", str.toLatin1().constData());
	elem.setText(str);
	return true;
}

bool xmlHandler::fatalError(const QXmlParseException &exception)
{
	printf("FFZF\n");
	return true;
}
