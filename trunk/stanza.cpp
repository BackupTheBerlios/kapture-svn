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
/*	i = "";
	t = Jid();
	f = Jid();
	ty = "";
	k = BadStanza;
	doc = QDomDocument("");
	tn = "";
*/
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

	n = doc;
	
//	i = id;
//	ty = type;
//	t = Jid(to);
//	tn = kindToTagName(kind);
}

//Stanza::Stanza(const QByteArray &node)
Stanza::Stanza(const QDomNode &node)
{
	n = node;
/*	i = "";
	t = Jid();
	f = Jid();
	ty = "";
	
	tn = node.localName();
	k = tagNameToKind(tn);
	if (k == BadStanza)
	{
		return;
	}

	f = Jid(s.attribute("from"));
	t = Jid(s.attribute("to"));
	i = s.attribute("id");
	ty = s.attribute("type");
	ns = s.namespaceURI();
*/
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
	//f = from;
	if (from.isValid())
		n.toElement().setAttribute("from", from.full());
}

void Stanza::setTo(const Jid& to)
{
	//t = to;
	if (to.isValid())
		 n.toElement().setAttribute("to", to.full());
}

void Stanza::setId(const QString& id)
{
	//i = id;
	if (!id.isEmpty())
		 n.toElement().setAttribute("id", id);
}

void Stanza::setType(const QString& type)
{
	//ty = type;
	if (!type.isEmpty())
		 n.toElement().setAttribute("type", type);
}

void Stanza::setKind(Kind kind)
{
	//k = kind;
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

/*void Stanza::setupPresence(QDomElement s)
{
	ns = s.attribute("xmlns");
	to = s.attribute("to");
	from = s.attribute("from");
	type = s.attribute("type", "available");
	printf("ns = %s\nto = %s\nfrom = %s\ntype = %s\n", ns.toLatin1().constData(), to.toLatin1().constData(), from.toLatin1().constData(), type.toLatin1().constData());

	printf("Changing the presence of %s\n", from.toLatin1().constData());

	if (s.hasChildNodes())
		s = s.firstChildElement();
	else
		printf("error ! ");

	//printf("This node is %s\n", s.localName().toLatin1().constData());
	
	while (!s.isNull())
	{
		QString tag = s.localName();
		// !!! DO NOT MODIFY s BEFORE "s = s.nextSibling().toElement();" !!!
		if (tag == "priority")
		{
			if (s.firstChild().isText())
			{
				priority = s.firstChild().toText().data();
				printf("Priority =  %s\n", priority.toLatin1().constData());
			}
			else
			{
				printf("Error, no priority value.\n");
			}
		}
		
		if (tag == "show")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			show = s.firstChild().toText().data();
			printf("Show =  %s\n", show.toLatin1().constData());
		}
		if (tag == "status")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			status = s.firstChild().toText().data();
			printf("status =  %s\n", status.toLatin1().constData());
			
		}
		
		if (tag == "x" && s.namespaceURI() == "vcard-temp:x:update") 
		{
			printf("vcard update !\n");
			QDomNodeList vcardData = s.childNodes();
			for (int i = 0; i < vcardData.count(); i++)
			{
				if (vcardData.at(i).localName() == "nickname")
				{
					nickname = vcardData.at(i).firstChild().toText().data();
				}
			}
		}

		s = s.nextSibling().toElement();
	}

}

void Stanza::setupMessage(QDomElement s)
{
	to = s.attribute("to");
	from = s.attribute("from");
	type = s.attribute("type", "normal");

	if (s.hasChildNodes())
		s = s.firstChildElement();
	else
		printf("error ! ");
	
		// !!! DO NOT MODIFY s BEFORE s = s.nextSibling().toElement(); !!!
	while (!s.isNull())
	{
		QString tag = s.localName();
		if (tag == "subject")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			subject = s.firstChild().toText().data();
			printf("subject =  %s\n", subject.toLatin1().constData());
		}
		
		if (tag == "body")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			message = s.firstChild().toText().data();
			printf("message =  %s\n", message.toLatin1().constData());
		}
		
		if (tag == "thread")
		{
			if (s.firstChild().isText())
				printf("OK, it is text !\n");
			thread = s.firstChild().toText().data();
			printf("thread =  %s\n", thread.toLatin1().constData());
		}
		if (tag == "composing")
		{
			isComposing = true;
		}
		if (tag == "paused")
		{
			isComposing = false;
		}
		
		s = s.nextSibling().toElement();
	}
	printf("New message recieved from %s. (type = %s) :\n %s\n", from.toLatin1().constData(), type.toLatin1().constData(), message.toLatin1().constData());
	
	 *
	 * Xmpp will have to manage different types of message (chat, error, groupchat, headline, normal)
	 *
}

QStringList Stanza::getFeatures()
{
	QStringList f = features;
	features.clear();
	return f;
}

int Stanza::getAction()
{
	int a = action;
	action = None;
	return a;
}

QString Stanza::getFrom()
{
	return from;
}

QString Stanza::getTo()
{
	return to;
}

QString Stanza::getMessage()
{
	QString ret = message;
	message.clear();
	return ret;
}

QString Stanza::getNickname()
{
	QString n = nickname;
	nickname = "";
	return n;
}

QStringList Stanza::getContacts()
{
	QStringList ret = contacts;
	contacts.clear();
	return ret;
}

QString Stanza::getId()
{
	return id;
}

QString Stanza::getType()
{
	return type;
}


QString Stanza::getStatus()
{
	return status;
}

QString Stanza::getShow()
{
	return show;
}

QStringList Stanza::getNicknameList()
{
	return nicknames;
}
*/
