/*
 *      Kapture - xmlhandler.cpp
 *
 *      Copyright (C) 2006-2007
 *          Detlev Casanova (detlev.casanova@gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

#include "xmlhandler.h"
#include "event.h"
#include <QtXml>

XmlHandler::XmlHandler()
{
	depth = 0;
}

XmlHandler::~XmlHandler()
{

}

bool XmlHandler::startElement(const QString &namespaceURI, const QString &,
			  const QString &qName, const QXmlAttributes &attributes)
{
	if (depth == 0)
	{
		Event *e = new Event(Event::Stream);
		eventList << e;
	}
	else
	{
		QDomElement e = doc.createElementNS(namespaceURI, qName);
		for (int i = 0; i < attributes.count(); ++i)
			e.setAttributeNS(attributes.uri(i), attributes.qName(i),
				attributes.value(i));

		if (!current.isNull())
		{
			current.appendChild(e);
			current = e;
		}
		else
		{
			root = e;
			current = e;
		}
	}

	depth++;
	return true;
}

bool XmlHandler::endElement(const QString&, const QString&, const QString&)
{
	depth--;
	
	if (depth == 1)
	{
		Event *e = new Event(Event::Stan, root);
		eventList << e;
//		printf("NEW STANZA EVENT\n");
		current = QDomElement();
		root = QDomElement();
		return true;
	}

	if (depth == 0)
	{
		Event *e = new Event(Event::EndOfStream);
		eventList << e;
//		printf("NEW ENDSREAM EVENT\n");
		return true;
	}
	current = current.parentNode().toElement();
	return true;
}

bool XmlHandler::characters(const QString& value)
{
	QDomText text = current.lastChild().toText();

	if (text.isNull())
	{
		text = doc.createTextNode(value);
		current.appendChild(text);
	}
	else
		text.appendData(value);
	return true;
}

bool XmlHandler::fatalError(const QXmlParseException &exception)
{
	printf(" ! Fatal error : %s.\n", exception.message().toLatin1().constData());
	return true;
}

Event* XmlHandler::read()
{
	if (eventList.empty())
		return NULL;

	return eventList.takeFirst();
}

