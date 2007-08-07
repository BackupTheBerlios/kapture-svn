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
	QString id = "roster_1";
	QString to = "";
	Stanza *s = new Stanza(Stanza::IQ, type, id, to);
	s->setFrom(j);
	QDomDocument d("");
	QDomElement c = d.createElement("query");
	c.setAttribute("xmlns", "jabber:iq:roster");
	s->appendChild(c);
	printf("Ask for roster : %s\n", s->data().constData());
	
	p->write(*s);
}

bool RosterTask::canProcess(const Stanza& s) const
{
	if (s.id() == "roster_1" && s.type() == "result")
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
	QDomElement query = s.document().firstChildElement().firstChildElement();
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
	QString id = "";
	QString to = "";
	Stanza *stanza = new Stanza(Stanza::Presence, type, id, to);
	QDomDocument doc("");
	QDomElement e = doc.createElement("presence");
	
	if (show != "")
	{
		QDomElement s = doc.createElement("show");
		QDomText val = doc.createTextNode(show);
		s.appendChild(val);
		stanza->appendChild(s);
	}
	
	if (status != "")
	{
		QDomElement s = doc.createElement("status");
		QDomText val = doc.createTextNode(status);
		s.appendChild(val);
		stanza->appendChild(s);
	}
	
	p->write(*stanza);
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
	QDomDocument doc = stanza.document();

	QDomElement s = doc.firstChildElement();
	if (!s.hasChildNodes())
	{
		show = "";
		status = "";
		emit presenceFinished();
		return;
	}

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
	QDomElement s = stanza.document().firstChildElement();
	
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
	QString id = "mess_1"; //FIXME:Must send a random string.
	Jid to = message.to();
	Stanza *stanza = new Stanza(Stanza::Message, message.type(), id, to.full());

	QDomDocument doc("");
	QDomElement e = doc.createElement("message"); // Just to create childs.
	
	QDomElement s = doc.createElement("body");
	QDomText val = doc.createTextNode(message.message());
	s.appendChild(val);
	stanza->appendChild(s);

	// TODO:Thread, subjects... later.
	
	p->write(*stanza);
	emit finished();
}
