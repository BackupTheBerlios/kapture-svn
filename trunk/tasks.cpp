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

#include <QTcpServer>

#include "tasks.h"

#define XMLNS_FEATURENEG "http://jabber.org/protocol/feature-neg"
#define XMLNS_SI "http://jabber.org/protocol/si"
#define XMLNS_FILETRANSFER "http://jabber.org/protocol/si/profile/file-transfer"

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
// StreamTask
//-------------------------------------
/*
 *
 */

#define XMLNS_DISCO "http://jabber.org/protocol/disco#info"

StreamTask::StreamTask(Task* parent, Xmpp *xmpp, const Jid& t)
	:Task(parent)
{
	p = xmpp;
	to = t;
}

StreamTask::~StreamTask()
{

}

bool StreamTask::canProcess(const Stanza& s) const
{
	if (s.kind() != Stanza::IQ)
		return false;
	
	QString ns = s.node().firstChildElement().firstChildElement().namespaceURI();
	
	if (ns == XMLNS_DISCO)
		return true;
	

	printf("Type = %s, Id = %s (expected %s), namespace = %s\n", s.type().toLatin1().constData(),
			s.id().toLatin1().constData(),
			id.toLatin1().constData(),
			s.node().toElement().namespaceURI().toLatin1().constData());

	if (s.type() == "result" &&
	    s.id() == id &&
	    s.node().toElement().namespaceURI() == "jabber:client")
	    	return true;
	/*
	 * Other namespaces should be supported here.
	 * http://jabber.org/protocol/disco#info is only for features discovering.
	 */
	return false;
}

void StreamTask::processStanza(const Stanza& s)
{
	switch (state)
	{
	case WaitDiscoInfo:
		if (s.type() == "result" && 
	 	    s.kind() == Stanza::IQ &&
		    s.id() == id)
		{
			printf("Ok, result received.\n");
			//printf("TEST = %s\n", s.node().localName().toLatin1().constData());
			//if (s.node().hasChildNodes())
			//	printf("It has childNodes !!!!!\n");
			QDomNode node = s.node().firstChild();
			//if (!node.isNull())
			//	printf("TEST = %s\n", node.localName().toLatin1().constData());
			if (node.localName() != "query" || node.namespaceURI() != XMLNS_DISCO)
			{
			//	printf("localName = %s, namespace = %s\n", node.localName().toLatin1().constData(), node.namespaceURI().toLatin1().constData());
				//emit error();
				printf("Bad stanza. Stoppping here.\n");
				//emit finished();
				return;
			}
			node = node.firstChild();
			while (!node.isNull())
			{
				if (node.localName() == "feature")
					featureList << node.toElement().attribute("var");
				node = node.nextSibling();
			}
			emit infoDone();
		}
		break;
	case WaitAcceptFileTransfer:
		QDomNode node = s.node().firstChild();
		if (node.localName() != "si")
		{
			printf("Not SI tag, stop.\n");
			//emit error();
			return;
		}
		if (node.localName() == "file")
			node = node.nextSibling();
		node = node.firstChild();
		if (node.localName() != "feature")
		{
			printf("Not FEATURE tag, stop.\n");
			//emit error();
			return;
		}
		node = node.firstChild();
		if (node.localName() != "x")
		{
			printf("Not X tag, stop.\n");
			//emit error();
			return;
		}
		node = node.firstChild();
		if (node.localName() != "field")
		{
			printf("Not FIELD tag, stop.\n");
			//emit error();
			return;
		}
		node = node.firstChild();
		if (node.localName() != "value")
		{
			printf("Not VALUE tag, stop.\n");
			//emit error();
			return;
		}
		profileToUse = node.firstChild().toText().data();
		printf("Ok, Using %s profile to transfer file.\n", profileToUse.toLatin1().constData());
		emit finished();
		break;
		
	}
}

void StreamTask::discoInfo()
{
/*
 * Stream Initiation
 *  - Discovers if Receiver implements the desired profile.
 *  - Offers a stream initiation.
 *  - Receiver accepts stream initiation.
 *  - Sender and receiver prepare for using negotiated profile and stream.
 *  See XEP 0095 : http://www.xmpp.org/extensions/xep-0095.html
 */
	id = randomString(8);
	Stanza stanza(Stanza::IQ, "get", id, to.full());
	QDomNode node = stanza.node().firstChild();
	QDomDocument d = node.ownerDocument();
	QDomElement query = d.createElement("query");
	query.setAttribute("xmlns", XMLNS_DISCO);
	node.appendChild(query);

	state = WaitDiscoInfo;
	p->write(stanza);
}

bool StreamTask::supports(const QString& profile)
{
	for (int i = 0; i < featureList.count(); i++)
		if (featureList.at(i) == profile)
			return true;
	return false;
}

QString StreamTask::negProfile() const
{
	return profileToUse;
}

void StreamTask::initStream(const QFile& f)
{
//	f = file;
	id = randomString(6);
	Stanza stanza(Stanza::IQ, "set", id, to.full());
	QDomNode node = stanza.node().firstChild();
	QDomDocument doc("");
	
	QDomElement si = doc.createElement("si");
	si.setAttribute("xmlns", XMLNS_SI);
	si.setAttribute("id", SID = randomString(2));
	si.setAttribute("profile", XMLNS_FILETRANSFER);
	
	QDomElement file = doc.createElement("file");
	file.setAttribute("xmlns", XMLNS_FILETRANSFER);
	file.setAttribute("name", f.fileName());
	file.setAttribute("size", QString("%1").arg((int)f.size()));

	QDomElement feature = doc.createElement("feature");
	feature.setAttribute("xmlns", XMLNS_FEATURENEG);

	QDomElement x = doc.createElement("x");
	x.setAttribute("xmlns", "jabber:x:data");
	x.setAttribute("type", "form");

	QDomElement field = doc.createElement("field");
	field.setAttribute("var", "stream-method");
	field.setAttribute("type", "list-single");

	QDomElement option1 = doc.createElement("option");

	QDomElement value1 = doc.createElement("value");

	QDomText bytestream = doc.createTextNode("http://jabber.org/protocol/bytestreams"); //Not supported yet

	node.appendChild(si);
	si.appendChild(file);
	file.appendChild(feature);
	feature.appendChild(x);
	x.appendChild(field);
	field.appendChild(option1);
	option1.appendChild(value1);
	value1.appendChild(bytestream);
	
	//printf("Node = %s\n", stanza.node().toDocument().toString().toLatin1().constData());
	//rintf("Node tag = %s\n", node.localName().toLatin1().constData());
	state = WaitAcceptFileTransfer;
	p->write(stanza);
}

Jid StreamTask::toJid() const
{
	return to;
}

QString StreamTask::sid() const
{
	return SID;
}

//QFile StreamTask::file() const
//{
//	return f;
//}

//----------------------------------------
// FileTransferTask
//----------------------------------------

FileTransferTask::FileTransferTask(Task *parent, const Jid& t, Xmpp *xmpp)
	:Task(parent)
{
	to = t;
	p = xmpp;
	writtenData = 0;
	prc = 0;
	prc2 = 0;
}

FileTransferTask::~FileTransferTask()
{}

bool FileTransferTask::canProcess(const Stanza& s) const
{
	printf("Type = %s, Id = %s (expected %s), namespace = %s\n", s.type().toLatin1().constData(),
			s.id().toLatin1().constData(),
			id.toLatin1().constData(),
			s.node().toElement().namespaceURI().toLatin1().constData());
	if (s.kind() == Stanza::IQ && s.id() == id)
		return true;
	return false;
}

#define STEP 512

void FileTransferTask::processStanza(const Stanza&)
{
	f->open(QIODevice::ReadOnly);
	if (f->size() < STEP)
	{
		socks5Socket->write(f->readAll());
		writtenData = f->size();
		disconnect(socks5Socket);
		socks5Socket->disconnectFromHost();
		emit finished();
	}
	else
	{
		socks5Socket->write(f->read(STEP));
		connect(socks5Socket, SIGNAL(bytesWritten(qint64)), this, SLOT(writeNext(qint64)));
	}
}

void FileTransferTask::writeNext(qint64 sizeWritten)
{
	writtenData = writtenData + sizeWritten;
	QByteArray data = f->read(STEP);
	if (data.size() != 0)
		socks5Socket->write(data);
	else
	{
		socks5Socket->disconnect();
		socks5Socket->disconnectFromHost();
		emit finished();
	}
	//TODO: a rubbish bin is cleaner than that !
	prc = (int)(((float)writtenData/(float)f->size())*100);
	if (prc2 != (int)(((float)writtenData/(float)f->size())*100))
		printf("Pourcentage : %d %%.\n", prc);
	prc2 = (int)(((float)writtenData/(float)f->size())*100);
}

void FileTransferTask::start(const QString& profile, const QString& SID, const QString& file)
{
	if (profile == "http://jabber.org/protocol/bytestreams")
		startByteStream(SID);
	s = SID;
	printf("File name = %s\n", file.toLatin1().constData());
	f = new QFile(file);
}

void FileTransferTask::startByteStream(const QString &SID)
{
//TODO: Rename test and truc.
	// Get network address.
	id = randomString(6);
	Stanza stanza(Stanza::IQ, "set", id, to.full());
	stanza.setFrom(p->node());
	QDomDocument doc("");
	QDomElement query = doc.createElement("query");
	query.setAttribute("xmlns", "http://jabber.org/protocol/bytestreams");
	query.setAttribute("sid", SID);
	query.setAttribute("mode", "tcp");
	
	QNetworkInterface *truc = new QNetworkInterface();
	for (int i = 0; i < truc->allAddresses().count(); i++)
	{
		printf("IP : %s\n", truc->allAddresses().at(i).toString().toLatin1().constData());
		QDomElement streamHost = doc.createElement("streamhost");
		streamHost.setAttribute("jid", p->node().full());
		streamHost.setAttribute("host", truc->allAddresses().at(i).toString());
		streamHost.setAttribute("port", "8015");
		query.appendChild(streamHost);
	}
	
	stanza.node().firstChild().appendChild(query);
	test = new QTcpServer();
	test->listen(QHostAddress("127.0.0.1"), 8015);
	test->listen(QHostAddress("192.168.3.2"), 8015);
	test->listen(QHostAddress("192.168.123.150"), 8015);
	connect(test, SIGNAL(newConnection()), this, SLOT(newConnection()));
	p->write(stanza);
}

void FileTransferTask::newConnection()
{
	printf("New Connection Received.\n");
	socks5 = new Socks5(s, p->node(), to);
	connect(socks5, SIGNAL(readyRead()), this, SLOT(readS5()));
	socks5Socket = test->nextPendingConnection();
	connect(socks5Socket, SIGNAL(readyRead()), this, SLOT(dataAvailable()));
}

void FileTransferTask::dataAvailable() //Should be in a different class (SOCKS5)
{
	QByteArray data = socks5Socket->readAll();
	printf("Data (%d bytes)\n", data.size());
	socks5->write(data);

	/*if (data.at(0) == 0x5)
		printf("Ok, SOCKS5.\n");
	else
		printf("OUCH ! Bad SOCKS5 request.\n");
	for (int i = 0; i < data.size(); i++)
	{
		printf("0x%x ", data.at(i));
	}
	printf("\n");
	char answer1 = 0x5;
	char answer2 = 0x0;
	QByteArray truc = QByteArray(1, answer1);
	truc.append(answer2);
	sock5->write(truc);*/
}

void FileTransferTask::readS5()
{
	socks5Socket->write(socks5->read());
}
