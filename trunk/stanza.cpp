/*
 *      Kapture
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

#include "stanza.h"
#include <QString>


Stanza::Stanza()
{

}

Stanza::Stanza(Kind kind, const QString& type, const QString& id, const QString& to, const QString& namespaceURI)
{
	QDomDocument doc("");
	doc.appendChild(doc.createElement(kindToTagName(kind)));
	
	if (!type.isEmpty())
		doc.documentElement().setAttribute("type", type);
	if (!id.isEmpty())
		doc.documentElement().setAttribute("id", id);
	if (!to.isEmpty())
		doc.documentElement().setAttribute("to", to);
	if (!namespaceURI.isEmpty())
		doc.documentElement().setAttribute("xmlns", namespaceURI);

	n = doc; //FIXME : should be doc.firstChild().
	
//	i = id;
//	ty = type;
//	t = Jid(to);
//	tn = kindToTagName(kind);
}

//Stanza::Stanza(const QByteArray &node)
Stanza::Stanza(const QDomNode &node)
{
	n = node;
}

Stanza::~Stanza()
{

}

Stanza::Kind Stanza::tagNameToKind(QString tagName) const
{
	if (tagName == "iq")
		return IQ;
	if (tagName == "presence")
		return Presence;
	if (tagName == "message")
		return Message;
	return BadStanza;
}

QString Stanza::kindToTagName(Kind kind) const
{
	if (kind == IQ)
		return "iq";
	if (kind == Presence)
		return "presence";
	if (kind == Message)
		return "message";
	return "";
}

void Stanza::setFrom(const Jid& from)
{
	if (from.isValid())
		n.firstChild().toElement().setAttribute("from", from.full());
}

void Stanza::setTo(const Jid& to)
{
	if (to.isValid())
		 n.toElement().setAttribute("to", to.full());
}

void Stanza::setId(const QString& id)
{
	if (!id.isEmpty())
		 n.toElement().setAttribute("id", id);
}

void Stanza::setType(const QString& type)
{
	if (!type.isEmpty())
		 n.toElement().setAttribute("type", type);
}

void Stanza::setKind(Kind kind)
{
	 n.toElement().setTagName(kindToTagName(kind));
}

Jid Stanza::from() const
{
	Jid f = Jid(n.toElement().attribute("from"));
	return f;
}

Jid Stanza::to() const
{
	Jid t = Jid(n.toElement().attribute("to"));
	return t;
}

QString Stanza::id() const
{
	return n.toElement().attribute("id");
}

QString Stanza::type() const
{
	return n.toElement().attribute("type");
}

Stanza::Kind Stanza::kind() const
{
	return tagNameToKind(n.localName());
}

QDomNode Stanza::node() const
{
	return n;
}
