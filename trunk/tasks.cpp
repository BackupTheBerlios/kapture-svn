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

#include "tasks.h"

RosterTask::RosterTask(Task* parent)
	:Task(parent)
{

}

RosterTask::~RosterTask()
{

}

void RosterTask::getRoster(Xmpp* p, Jid& j)
{
	QString type = "get";
	id = randomString(6);
	
	Stanza stanza(Stanza::IQ, type, id, QString());
	QDomDocument doc("");
	stanza.setFrom(j);
	QDomElement c = doc.createElement("query");
	c.setAttribute("xmlns", "jabber:iq:roster");
	stanza.node().firstChild().appendChild(c);
	
	p->write(stanza);
}

bool RosterTask::canProcess(const Stanza& s) const
{
	if (s.id() == id && s.type() == "result")
		return true;
	else
		return false;
}

void RosterTask::processStanza(const Stanza& s)
{
	printf("void RosterTask::processStanza(const Stanza& s)\n");
	QString j;
	QString n;
	QString subs;
	QDomElement query = s.node().firstChildElement();
	if (query.localName() == "query")
	{
		QDomNodeList items = query.childNodes();
		for (int i = 0; i < items.count(); i++)
		{
			j = items.at(i).toElement().attribute("jid");
			n = items.at(i).toElement().attribute("name");
			subs = items.at(i).toElement().attribute("subscription");

			r.addContact(j, n, subs);

			printf("New Roster contact : %s (subscription : %s)\n", 
				items.at(i).toElement().attribute("jid").toLatin1().constData(),
				items.at(i).toElement().attribute("subscription").toLatin1().constData());
		}
	}
	emit finished();
}

Roster RosterTask::roster() const
{
	return r;
}

//----------------------------------
// PresenceTask
//----------------------------------

/*
 * Manage this resource presence and status.
 * Changes it with the method setPresence().
 */

// FIXME:Maybe no task is needed for that...

PresenceTask::PresenceTask(Task* parent)
	:Task(parent)
{

}

PresenceTask::~PresenceTask()
{

}

void PresenceTask::setPresence(Xmpp* p, const QString& show, const QString& status, const QString& type)
{
	Stanza stanza(Stanza::Presence, type, QString(), QString());
	QDomNode node = stanza.node();
	QDomDocument doc = node.ownerDocument();

	QDomElement e = doc.createElement("presence");
	
	if (show != "")
	{
		QDomElement s = doc.createElement("show");
		QDomText val = doc.createTextNode(show);
		s.appendChild(val);
		node.appendChild(s);
	}
	
	if (status != "")
	{
		QDomElement s = doc.createElement("status");
		QDomText val = doc.createTextNode(status);
		s.appendChild(val);
		node.appendChild(s);
	}
	
	p->write(stanza);
	emit finished();
}

//----------------------------------
// PullPresenceTask
//----------------------------------

/*
 * Received and process new presences from contacts.
 */

PullPresenceTask::PullPresenceTask(Task* parent)
	:Task(parent)
{
	printf("PullPresenceTask::PullPresenceTask(Task* parent)\n");

}

PullPresenceTask::~PullPresenceTask()
{

}

bool PullPresenceTask::canProcess(const Stanza& s) const
{
	printf("Kind = %s\n", s.kind() == Stanza::Presence ? "Presence" : "NoPresence");
	if (s.kind() == Stanza::Presence)
		return true;
	return false;
}

void PullPresenceTask::processStanza(const Stanza& stanza)
{
	printf("PullPresenceTask:: : %s\n", stanza.from().full().toLatin1().constData());
	from = stanza.from();
	Jid to = stanza.to();
	type = stanza.type() == "" ? "available" : stanza.type();
	QDomNode node = stanza.node();

	QDomElement s = node.toElement();//.firstChildElement();
	if (!s.hasChildNodes())
	{
		show = "";
		status = "";
		emit presenceFinished();
		return;
	}
	printf("Set presence\n");
	s = s.firstChildElement();
	while(!s.isNull())
	{
		if (s.localName() == "show")
		{
			if (s.firstChild().isText())
				show = s.firstChild().toText().data();
			else
				show = "";
		}
		if (s.localName() == "status")
		{
			if (s.firstChild().isText())
				status = s.firstChild().toText().data();
			else
				status = "";
		}
		/*
		 * No priority managed yet, 
		 * no VCard managed yet.
		 */
		s = s.nextSibling().toElement();
	}
	emit presenceFinished();
}

Presence PullPresenceTask::getPresence() const
{
	return Presence(from, type, status, show);
}

//-----------------------------
// PullMessageTask
//-----------------------------

/*
 * Receives and process new messages from contacts.
 */

PullMessageTask::PullMessageTask(Task* parent)
	:Task(parent)
{

}

PullMessageTask::~PullMessageTask()
{

}

bool PullMessageTask::canProcess(const Stanza& s) const
{
	if (s.kind() == Stanza::Message)
		return true;
	return false;
}

void PullMessageTask::processStanza(const Stanza& stanza)
{
	ty = stanza.type();
	f = stanza.from();
	t = stanza.to();
	QDomElement s = stanza.node().toElement();
	
	if (!s.hasChildNodes())
		m = "";
	else
		s = s.firstChildElement();
	
	while (!s.isNull())
	{
		if (s.localName() == "body")
		{
			/*
			 * FIXME:
			 * There can be more than one body child,
			 * giving traductions of the message in other languages
			 */
			if (s.firstChild().isText())
				m = s.firstChild().toText().data();
			else
				m = "";
		}

		if (s.localName() == "subject") // Not used yet.
		{
			if (s.firstChild().isText())
				sub = s.firstChild().toText().data();
			else
				sub = "";
		}
		
		if (s.localName() == "thread") // Not used yet.
		{
			if (s.firstChild().isText())
				thr = s.firstChild().toText().data();
			else
				thr = "";
		}
		s = s.nextSibling().toElement();
	}
	emit messageFinished();
}

Message PullMessageTask::getMessage()
{
	return Message(f, t, m, ty, sub, thr);
}

//-------------------------------------
// MessageTask
//-------------------------------------

MessageTask::MessageTask(Task* parent)
	:Task(parent)
{}

MessageTask::~MessageTask()
{}

void MessageTask::sendMessage(Xmpp* p, const Message& message)
{
	QString id = randomString(6); //FIXME:Must send a random string.
	Jid to = message.to();
	Stanza stanza(Stanza::Message, message.type(), id, to.full());
	QDomDocument doc("");
	QDomElement s = doc.createElement("body");
	QDomText val = doc.createTextNode(message.message());
	s.appendChild(val);
	stanza.node().firstChild().appendChild(s);

	// TODO:Thread, subjects... later.
	
	p->write(stanza);
	emit finished();
}

//-------------------------------------
// FileTransferTask
//-------------------------------------
/*
 *
 */

#define XMLNS_DISCO "http://jabber.org/protocol/disco#info"

FileTransferTask::FileTransferTask(Task* parent)
	:Task(parent)
{

}

FileTransferTask::~FileTransferTask()
{

}

void FileTransferTask::transferFile(Xmpp* p, const Jid& to, const QFile& file)
{
/*Init stream initiation...*/
/*
 * <feature var='http://jabber.org/protocol/si'/>
 * <feature var='http://jabber.org/protocol/si/profile/file-transfer'/>
 */
	//f = file;
	Stanza stanza(Stanza::IQ, "get", "askInfo1", to.full());
	QDomNode node = stanza.node();
	QDomDocument d = node.ownerDocument();
	QDomElement query = d.createElement("query");
	query.setAttribute("xmlns", XMLNS_DISCO);
	node.appendChild(query);

	state = WaitDiscoInfo;
	p->write(stanza);
}

bool FileTransferTask::canProcess(const Stanza& s) const
{
	if (s.kind() != Stanza::IQ)
		return false;
	QString ns = s.node().firstChildElement().firstChildElement().namespaceURI();
	if (ns == "http://jabber.org/protocol/disco#info")
		return true;
	/*
	 * Other namespaces should be supported here.
	 * http://jabber.org/protocol/disco#info is only for features discovering.
	 */
	return false;
}

void FileTransferTask::processStanza(const Stanza& s)
{
	switch (state)
	{
	case WaitDiscoInfo:
		if (s.type() == "result")
			printf("Ok, result received.\n");
		break;
	}
}

void FileTransferTask::initStream()
{
/*
 * Stream Initiation
 *  - Discovers if Receiver implements the desired profile.
 *  - Offers a stream initiation.
 *  - Receiver accepts stream initiation.
 *  - Sender and receiver prepare for using negotiated profile and stream.
 *  See XEP 0095 : http://www.xmpp.org/extensions/xep-0095.html
 */
}
