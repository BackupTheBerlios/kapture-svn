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
#define XMLNS_DISCO_INFO "http://jabber.org/protocol/disco#info"
#define XMLNS_DISCO_ITEMS "http://jabber.org/protocol/disco#items"
#define XMLNS_BYTESTREAMS "http://jabber.org/protocol/bytestreams"
#define XMLNS_JINGLE "http://www.xmpp.org/extensions/xep-0166.html#ns"
#define XMLNS_VIDEO "http://www.xmpp.org/extensions/xep-0180.html#ns"
#define XMLNS_RTP "http://www.xmpp.org/extensions/xep-0176.html#ns-udp"
#define TIMEOUT 30000

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

}

PullPresenceTask::~PullPresenceTask()
{

}

bool PullPresenceTask::canProcess(const Stanza& s) const
{
	if (s.kind() == Stanza::Presence)
		return true;
	return false;
}

void PullPresenceTask::processStanza(const Stanza& stanza)
{
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
	
	//printf("Type = %s, Id = %s (expected %s), namespace = %s\n", s.type().toLatin1().constData(),
	//		s.id().toLatin1().constData(),
	//		id.toLatin1().constData(),
	//		s.node().toElement().namespaceURI().toLatin1().constData());

	if (s.id() == id /*&&
	    s.node().toElement().namespaceURI() == "jabber:client"*/)
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
		if (s.type() == "result")
		{
			QDomNode node = s.node().firstChild();
			if (node.localName() != "query" || node.namespaceURI() != XMLNS_DISCO_INFO)
			{
				printf("[STREAMTASK] Bad stanza. Stoppping here.\n");
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
		if (s.type() == "error")
		{
			//emit error(DiscoInfoError);
		}
		break;
	case WaitAcceptFileTransfer:
		if (s.type() == "result")
		{
			QDomNode node = s.node().firstChild();
			if (node.localName() != "si")
			{
				printf("[STREAMTASK] Not SI tag, stop.\n");
				//emit error();
				return;
			}
			if (node.localName() == "file")
				node = node.nextSibling();
			node = node.firstChild();
			if (node.localName() != "feature")
			{
				printf("[STREAMTASK] Not FEATURE tag, stop.\n");
				//emit error();
				return;
			}
			node = node.firstChild();
			if (node.localName() != "x")
			{
				printf("[STREAMTASK] Not X tag, stop.\n");
				//emit error();
				return;
			}
			node = node.firstChild();
			if (node.localName() != "field")
			{
				printf("[STREAMTASK] Not FIELD tag, stop.\n");
				//emit error();
				return;
			}
			node = node.firstChild();
			if (node.localName() != "value")
			{
				printf("[STREAMTASK] Not VALUE tag, stop.\n");
				//emit error();
				return;
			}
			profileToUse = node.firstChild().toText().data();
			printf("[STREAMTASK] Ok, Using %s profile to transfer file.\n", profileToUse.toLatin1().constData());
			getProxies();
		}
		if (s.type() == "error")
		{
			QDomNode node = s.node().firstChild();
			if (node.localName() == "error")
			{
				int errCode = node.toElement().attribute("code", "1").toInt();
				QString errorString = node.firstChild().toText().data();
				emit error(errCode, errorString);
			}
		}
		break;
	case WaitProxies :
		if (s.type() == "result" && s.node().firstChildElement().firstChildElement().namespaceURI() == XMLNS_DISCO_ITEMS)
		{
			QDomNode node = s.node().firstChild();
			node = node.firstChild();
			while (!node.isNull())
			{
				if (node.localName() == "item")
					itemList << node.toElement().attribute("jid");
				node = node.nextSibling();
			}
			//emit infoDone();
		}
		printf("[STREAMTASK] Proxies are : \n");
		for (int i = 0; i < itemList.count(); i++)
			printf("[STREAMTASK]  * %s\n", itemList[i].toLatin1().constData());
		if (itemList.count() <= 0)
		{
			emit finished();
			return;
		}
		state = WaitIsProxy;
		isAProxy(itemList.takeFirst());
		break;
	case WaitIsProxy :
		if (s.type() == "result" && s.node().firstChildElement().firstChildElement().namespaceURI() == XMLNS_DISCO_INFO)
		{
			QDomNode node = s.node().firstChild();
			node = node.firstChild();
			printf("[STREAMTASK] node = %s\n", node.localName().toLatin1().constData());
			while (!node.isNull())
			{
				printf("[STREAMTASK] * Indentity = %s, Category = %s, type = %s\n", 
				        node.localName().toLatin1().constData(),
					node.toElement().attribute("category").toLatin1().constData(), 
					node.toElement().attribute("type").toLatin1().constData());

				if ((node.localName() == "identity") &&
				    (node.toElement().attribute("category") == "proxy") &&
				    (node.toElement().attribute("type") == "bytestreams"))
				{
					proxyList << s.node().toElement().attribute("from");
					proxyList2 << s.node().toElement().attribute("from");
				}
				node = node.nextSibling();
			}
			printf("[STREAMTASK] itemList.count() = %d\n", itemList.count());
			if (itemList.empty())
			{
				printf("[STREAMTASK]  ************* Number of Proxies = %d\n", proxyList.count());
				if (proxyList.count() <= 0)
				{
					emit finished();
					return;
				}
				else
				{
					state = WaitProxyIp;
					getProxyIp(proxyList.takeFirst());
				}
			}
			else
				isAProxy(itemList.takeFirst());
		}
		break;
	case WaitProxyIp :
		if (s.type() == "result" && s.node().firstChildElement().firstChildElement().namespaceURI() == XMLNS_BYTESTREAMS)
		{
			QDomNode node = s.node().firstChild();
			node = node.firstChild();
			if (node.localName() == "streamhost")
			{
				ipList << node.toElement().attribute("host", "UNKNOWN");
				portList << node.toElement().attribute("port", "UNKNOWN");
				//zeroconfList << node.toElement().attribute("zeroconf", "UNKNOWN");
			}
			if (proxyList.empty())
				emit finished();
			else
			{
				getProxyIp(proxyList.takeFirst());
			}
		}
	}
}

QStringList StreamTask::proxies() const
{
	return proxyList2;
}

QStringList StreamTask::ips() const
{
	return ipList;
}

QStringList StreamTask::ports() const
{
	return portList;
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
	query.setAttribute("xmlns", XMLNS_DISCO_INFO);
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

void StreamTask::getProxies()
{
/*
<iq type='get' 
    from='initiator@host1/foo'
    to='host1' 
    id='server_items'>
  <query xmlns='http://jabber.org/protocol/disco#items'/>
</iq>
*/
	id = randomString(8);
	Stanza stanza(Stanza::IQ, "get", id, p->node().domain());
	QDomNode node = stanza.node().firstChild();
	QDomDocument doc("");

	QDomElement query = doc.createElement("query");
	query.setAttribute("xmlns", XMLNS_DISCO_ITEMS);
	
	node.appendChild(query);
	state = WaitProxies;
	p->write(stanza);
}

void StreamTask::isAProxy(QString host)
{
	id = randomString(8);
	Stanza stanza(Stanza::IQ, "get", id, host);
	QDomNode node = stanza.node().firstChild();
	QDomDocument doc("");

	QDomElement query = doc.createElement("query");
	query.setAttribute("xmlns", XMLNS_DISCO_INFO);
	
	node.appendChild(query);
	p->write(stanza);
}

void StreamTask::getProxyIp(QString proxy)
{
	id = randomString(8);
	Stanza stanza(Stanza::IQ, "get", id, proxy);
	QDomNode node = stanza.node().firstChild();
	QDomDocument doc("");

	QDomElement query = doc.createElement("query");
	query.setAttribute("xmlns", XMLNS_BYTESTREAMS);
	
	node.appendChild(query);
	p->write(stanza);
}

void StreamTask::initStream(const QFile& f)
{
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
	QFileInfo fi(f.fileName());
	file.setAttribute("name", fi.fileName());
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

	QDomText bytestream = doc.createTextNode("http://jabber.org/protocol/bytestreams"); //Fully supported

	node.appendChild(si);
	si.appendChild(file);
	si.appendChild(feature);
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
	connectToProxy = true;
	isRecept = false;
}

FileTransferTask::~FileTransferTask()
{
	delete socks5Socket;
	delete socks5;
	// Remove all server sockets
	for (int i = 0; i < serverList.count(); i++)
	{
		delete serverList.at(i);
		serverList.removeAt(i);
	}
}

bool FileTransferTask::canProcess(const Stanza& s) const
{
	printf("Type = %s, Id = %s (expected %s), namespace = %s\n", s.type().toLatin1().constData(),
			s.id().toLatin1().constData(),
			id.toLatin1().constData(),
			s.node().toElement().namespaceURI().toLatin1().constData());
	if (s.kind() == Stanza::IQ && s.id() == id) //Check more....
		return true;
	return false;
}

#define STEP 512

void FileTransferTask::processStanza(const Stanza& s)
{
	if (s.type() != "result")
	{
		;
		//emit error();
	}

	QDomNode node = s.node().firstChild().firstChild();
	
	if (!connectToProxy)
	{
		f->open(QIODevice::ReadOnly);
		QFileInfo fi(f->fileName());
		QString str = fi.fileName();
		emit prcentChanged(to, str, 0); // Tell chatWin that the transfer begins
		writtenData = 0;
		socks5Socket->write(f->readAll());
		connect(socks5Socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWrittenSlot(qint64)));
		prc = 0;
		prc2 = 0;
	}
	else
	{
		if (node.localName() != "streamhost-used")
		{
			//emit error();
			return;
		}
		timeOut->stop(); // Connection received, no need to wait anymore.
		delete timeOut; // Unused now.
		
		printf("Must be using a proxy.\n");
		// Search the used proxy.
		int i;
		for (i = 0; i < proxies.count(); i++)
		{
			if (proxies.at(i) == node.toElement().attribute("jid"))
			{
				usedProxy = proxies.at(i);
				usedIP	  = ips.at(i);
				usedPort  = ports.at(i);
				break;
			}
		}
		socks5Socket = new QTcpSocket();
		socks5Socket->connectToHost(usedIP, usedPort.toLatin1().toInt());
		connect(socks5Socket, SIGNAL(connected()), this, SLOT(connectedToProxy()));
	}
}

void FileTransferTask::connectedToProxy()
{
	socks5 = new Socks5(s, p->node(), to);
	connect(socks5, SIGNAL(readyRead()), this, SLOT(readS5()));
	connect(socks5, SIGNAL(established()), this, SLOT(notifyStart()));
	connect(socks5Socket, SIGNAL(readyRead()), this, SLOT(dataAvailable()));
	socks5->connect();
}

void FileTransferTask::bytesWrittenSlot(qint64 sizeWritten)
{
	writtenData += sizeWritten;

	prc = (int)(((float)writtenData/(float)f->size())*100);
	if (prc2 != prc)
	{
		QFileInfo fi(f->fileName());
		QString str = fi.fileName();
		emit prcentChanged(to, str, prc);
	}
	prc2 = (int)(((float)writtenData/(float)f->size())*100);
	
	if (writtenData == f->size())
	{
		socks5Socket->disconnect();
		socks5Socket->disconnectFromHost();
		emit finished();
	}
}

void FileTransferTask::start(const QString& profile, const QString& SID, const QString& file,
			     const QStringList prox, const QStringList ip, const QStringList p)
{
	printf("%d proxies\n", prox.count());
	proxies = prox;
	ips = ip;
	ports = p;
	if (profile == "http://jabber.org/protocol/bytestreams")
		startByteStream(SID);
	
	s = SID;
	printf("File name = %s\n", file.toLatin1().constData());
	f = new QFile(file);
	fileName = file;
}

void FileTransferTask::startByteStream(const QString &SID)
{
	// Get network address.
	id = randomString(6);
	Stanza stanza(Stanza::IQ, "set", id, to.full());
	stanza.setFrom(p->node());
	QDomDocument doc("");
	QDomElement query = doc.createElement("query");
	query.setAttribute("xmlns", "http://jabber.org/protocol/bytestreams");
	query.setAttribute("sid", SID);
	query.setAttribute("mode", "tcp");
	
	timeOut = new QTimer();
	QNetworkInterface *interface = new QNetworkInterface();
	/* TODO:
	 * 	Should also add the external IP.
	 * 	For example, download it from
	 * 		http://www.swlink.net/~styma/REMOTE_ADDR.shtml
	 * 	or use a webservice.
	 * 	This is not prioritary.
	 */
	for (int i = 0; i < interface->allAddresses().count(); i++)
	{
		printf("IP : %s\n", interface->allAddresses().at(i).toString().toLatin1().constData());
		QDomElement streamHost = doc.createElement("streamhost");
		streamHost.setAttribute("jid", p->node().full());
		streamHost.setAttribute("host", interface->allAddresses().at(i).toString());
		streamHost.setAttribute("port", "8015");
		query.appendChild(streamHost);
		
		QTcpServer *tcpServer = new QTcpServer();
		tcpServer->listen(QHostAddress(interface->allAddresses().at(i).toString()), 8015);
		connect(tcpServer, SIGNAL(newConnection()), this, SLOT(newConnection()));
		serverList << tcpServer;
	}
	for (int i = 0; i < proxies.count(); i++)
	{
		printf("add a Proxy in the list\n");
		printf("IP : %s\n", ips.at(i).toLatin1().constData());
		QDomElement streamHost = doc.createElement("streamhost");
		streamHost.setAttribute("jid", proxies[i]);
		streamHost.setAttribute("host", ips[i]);
		streamHost.setAttribute("port", ports[i]);
		query.appendChild(streamHost);
	}
	timeOut->setInterval(TIMEOUT);
	connect(timeOut, SIGNAL(timeout()), this, SLOT(noConnection()));
	timeOut->start();

	stanza.node().firstChild().appendChild(query);
	p->write(stanza);
}

void FileTransferTask::noConnection()
{
	// Must retry with a proxy.
	printf("Unable to connect to the target, trying with a proxy.\n");
	emit notConnected();
}

void FileTransferTask::newConnection()
{
	printf("New Connection Received.\n");
	
	timeOut->stop(); // Connection received, no need to wait anymore.
	delete timeOut; // Unused now.

	QTcpServer *server;
	for (int i = 0; i < serverList.count(); i++)
	{
		if (serverList.at(i)->hasPendingConnections())
		{
			server = serverList.at(i);
			socks5Socket = serverList.at(i)->nextPendingConnection();
			// As this connection is a child of the server,
			// the QTcpServer cannot be destroied before we
			// are finished with the QTcpSocket.
			break;
		}
	}

	socks5 = new Socks5(s, p->node(), to);
	connect(socks5, SIGNAL(readyRead()), this, SLOT(readS5()));
	for (int i = 0; i < serverList.count(); i++)
	{
		serverList.at(i)->close();
	}
	connect(socks5Socket, SIGNAL(readyRead()), this, SLOT(dataAvailable()));
	connectToProxy = false;
}

void FileTransferTask::dataAvailable()
{
	QByteArray data = socks5Socket->readAll();
	if (!isRecept)
	{
		printf("[SOCKS5] Received : %s\n", data.toHex().constData());
		printf("Data (%d bytes)\n", data.size());
		socks5->write(data);
	}
	else
	{
		if (!fileOpened)
		{
			fileOut = new QFile(st);
			fileOut->open(QIODevice::WriteOnly | QIODevice::Append);
			fileOpened = true;
		}
		//printf("[FileTransferTask] Writing data in %s\n", st.toLatin1().constData());
		fileOut->write(data);
		writtenData += data.size();
		printf("[FileTransferTask] prc = %d, to = %s\n", (int)((float)writtenData/(float)filesize*100), to.full().toLatin1().constData());
		emit prcentChanged(to, filename, (int)((float)writtenData/(float)filesize*100));
	}
}

void FileTransferTask::readS5()
{
	QByteArray data = socks5->read();
	printf("[SOCKS5] Sent : %s (%d)\n", data.toHex().constData(), data.count());
	socks5Socket->write(data);
}

void FileTransferTask::notifyStart()
{
	id = randomString(8);
	Stanza stanza(Stanza::IQ, "set", id, usedProxy);
	QDomNode node = stanza.node().firstChild();
	stanza.setFrom(p->node());
	QDomDocument doc("");

	QDomElement query = doc.createElement("query");
	query.setAttribute("sid", s);
	query.setAttribute("xmlns", XMLNS_BYTESTREAMS);

	QDomElement activate = doc.createElement("activate");

	QDomText toText = doc.createTextNode(to.full());

	activate.appendChild(toText);
	query.appendChild(activate);
	node.appendChild(query);
	
	connectToProxy = false;
	p->write(stanza);
}

void FileTransferTask::setFileInfo(const QString& fileName, int fileSize)
{
	filename = fileName;
	filesize = fileSize;
}

void FileTransferTask::connectToHosts(QList<PullStreamTask::StreamHost> hostList, const QString& sid, const QString& i, const QString& saveTo)
{
	h = hostList;
	st = saveTo;
	id = i;
	s = sid;
	//PullStreamTask::StreamHost tytutuyty = h.takeFirst();
	if (h.count() > 0)
		tryToConnect(h.takeFirst());
	else
		cancel();
}

void FileTransferTask::cancel()
{

}

void FileTransferTask::tryToConnect(PullStreamTask::StreamHost hostData)
{
	socks5Socket = new QTcpSocket();
	QString host = hostData.host;
	usedJid = hostData.jid.full();
	printf("Connecting to %s.\n", host.toLatin1().constData());
	int port = hostData.port;
	/*
	 * On the target side, we don't care if streamhost is a proxy or the initiator.
	 */
	if (port != 0)
	{
		connect(socks5Socket, SIGNAL(connected()), this, SLOT(s5Connected()));
		connect(socks5Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(s5Error(QAbstractSocket::SocketError)));
		socks5Socket->connectToHost(host, port);
		printf("[FILETRANSFERTASK] host = %s, port = %d, jid = %s\n", host.toLatin1().constData(), port, usedJid.toLatin1().constData());
	}
	else
	{
		printf("[FILETRANSFERTASK] Use Zeroconf policy, not supported at all. Please restart Kapture.\n");
		// FIXME: Should emit an error here.
		// TODO: zeroconf connection.
	}
}

void FileTransferTask::s5Connected()
{
	fileOpened = false;
	/* Start connection to hostData.host with SOCKS5 */
	socks5 = new Socks5(s, to, p->node());
	isRecept = false;
	connect(socks5, SIGNAL(readyRead()), this, SLOT(readS5()));
	connect(socks5, SIGNAL(established()), this, SLOT(receptionNotify()));
	connect(socks5Socket, SIGNAL(readyRead()), this, SLOT(dataAvailable()));
	disconnect(socks5Socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(s5Error()));
	socks5->connect();
}

void FileTransferTask::s5Error(QAbstractSocket::SocketError e)
{
	if (e == QAbstractSocket::RemoteHostClosedError)
	{
		//FIXME:Should check if the whole file has been transfered.
		printf("Connection Closed.\n");
		socks5Socket->disconnect();
		socks5Socket->disconnectFromHost();
		fileOut->close();
		emit finished();
	}
	else
	{
		printf("Unable to transfer the file, ");
		delete socks5Socket;
		if (h.count() > 0)
		{
			printf("trying next streamhost.\n");
			tryToConnect(h.takeFirst());
		}
		else
		{
			printf("Cancelling [Not Implmented yet]\n");
			//TODO: send iq to tell Initiator that the connection couldn't be established.
		}
	}
}

void FileTransferTask::receptionNotify()
{
	isRecept = true;
	Stanza stanza(Stanza::IQ, "result", id, to.full());
	QDomNode node = stanza.node().firstChild();
	stanza.setFrom(p->node());
	QDomDocument doc("");

	QDomElement query = doc.createElement("query");
	query.setAttribute("xmlns", XMLNS_BYTESTREAMS);

	QDomElement streamhostused = doc.createElement("streamhost-used");
	streamhostused.setAttribute("jid", usedJid);

	node.appendChild(query);
	query.appendChild(streamhostused);

	p->write(stanza);
}

//-------------------------------
// PullStreamTask
//-------------------------------

PullStreamTask::PullStreamTask(Task *parent, Xmpp *xmpp)
	:Task(parent)
{
	p = xmpp;
	id = "";
}

bool PullStreamTask::canProcess(const Stanza& s) const
{
	if (s.kind() != Stanza::IQ)
		return false;

	if (s.type() == "get" &&
	    s.node().firstChildElement().namespaceURI() == XMLNS_DISCO_INFO)
		return true;
	
	if (s.type() == "set" &&
	    s.node().firstChildElement().namespaceURI() == XMLNS_SI)
		return true;

	if (s.type() == "set" && /* FIXME:Should check SID here */
	    s.node().firstChildElement().namespaceURI() == XMLNS_BYTESTREAMS)
	    	return true;

	return false;
}

void PullStreamTask::processStanza(const Stanza& s)
{
	id = s.id();
	if (s.node().firstChildElement().namespaceURI() == XMLNS_DISCO_INFO)
	{
		Stanza stanza(Stanza::IQ, "result", id, s.from().full());
		QDomNode node = stanza.node().firstChild();
		QDomDocument doc("");

		QDomElement query = doc.createElement("query");
		query.setAttribute("xmlns", XMLNS_DISCO_INFO);

		QDomElement identity = doc.createElement("identity");
		identity.setAttribute("category", "client");
		identity.setAttribute("type", "pc");

		// Supported protocols
		QDomElement feature = doc.createElement("feature");
		feature.setAttribute("var", XMLNS_SI);
		
		QDomElement feature1 = doc.createElement("feature");
		feature1.setAttribute("var", XMLNS_FILETRANSFER);
		
		QDomElement feature2 = doc.createElement("feature");
		feature2.setAttribute("var", XMLNS_BYTESTREAMS);

		query.appendChild(identity);
		query.appendChild(feature);
		query.appendChild(feature1);
		query.appendChild(feature2);
		node.appendChild(query);

		p->write(stanza);
	}

	if (s.node().firstChildElement().namespaceURI() == XMLNS_SI && s.type() == "set")
	{
		f = s.from();
		QDomNode node = s.node().firstChild();
		SID = node.toElement().attribute("id");
		
		node = node.firstChild();
		while (!node.isNull())
		{
			if (node.localName() == "file")
			{
				name = node.toElement().attribute("name");
				size = node.toElement().attribute("size").toInt();
				/*
				 * Hash is the MD5 sum of the file so, "NOHASH" cannot be it's value.
				 */
				hash = node.toElement().attribute("hash", "NOHASH");
				/*
				 * Date is the last modification time of the file specified using the DateTime profile so it can't be "NODATE"
				 */
				date = node.toElement().attribute("date", "NODATE");
				
				QDomNode n = node.firstChild();
				while (!n.isNull())
				{
					if (n.localName() == "desc")
					{
						desc = n.firstChild().toText().data();
					}
					n = n.nextSibling();
				}
			}
			if (node.localName() == "feature")
			{
				QDomNode n = node;
				if (n.firstChild().localName() != "x")
				{
					//emit error();
					printf("Error, No X Element, BAD STANZA !\n");
					return;
				}
				n = n.firstChild();

				if (n.firstChild().localName() != "field")
				{
					//emit error();
					printf("Error, No FIELD Element, BAD STANZA !\n");
					return;
				}
				n = n.firstChild();
				
				if (n.toElement().attribute("var") != "stream-method" || n.toElement().attribute("type") != "list-single")
				{
					//emit error();
					printf("Error, No FIELD Element, BAD STANZA !\n");
					return;
				}
				
				if (n.firstChild().localName() != "option")
				{
					//emit error();
					printf("Error, No OPTION Element, BAD STANZA !\n");
					return;
				}
				n = n.firstChild();
				

				while (!n.isNull())
				{
					pr.append(n.firstChild().firstChild().toText().data());
					n = n.nextSibling();
				}
			}
			node = node.nextSibling();
		}
		emit fileTransferIncoming();
	}
	
	if (s.node().firstChildElement().namespaceURI() == XMLNS_BYTESTREAMS && s.type() == "set")
	{
		QDomNode node = s.node().firstChild();
		if (node.toElement().attribute("sid") != SID || node.toElement().attribute("mode") != "tcp")
		{
			//emit error();
			return;
		}
		node = node.firstChild();
		while (!node.isNull())
		{
			if (node.localName() == "streamhost")
			{
				StreamHost streamhost;
				streamhost.jid = Jid(node.toElement().attribute("jid"));
				streamhost.host = node.toElement().attribute("host");
				streamhost.port = node.toElement().attribute("port", "0").toInt();
				streamHostList << streamhost;
			}
			node = node.nextSibling();
		}

		emit receiveFileReady();
	}
}

QList<PullStreamTask::StreamHost> PullStreamTask::streamHosts() const
{
	return streamHostList;
}

Jid PullStreamTask::from() const
{
	return f;
}

QString PullStreamTask::fileName() const
{
	return name;
}

int PullStreamTask::fileSize() const
{
	return size;
}

QString PullStreamTask::fileDesc() const
{
	return desc;
}

QString PullStreamTask::sid() const
{
	return SID;
}

QString PullStreamTask::lastId() const
{
	return id;
}

void PullStreamTask::ftDecline(const QString&, const Jid&)
{
	Stanza stanza(Stanza::IQ, "error", id, f.full());
	QDomNode node = stanza.node().firstChild();
	QDomDocument doc("");

	QDomElement error = doc.createElement("error");
	error.setAttribute("code", 403);

	QDomText text = doc.createTextNode("Declined");

	error.appendChild(text);
	node.appendChild(error);

	p->write(stanza);

}

QString PullStreamTask::saveFileName() const
{
	return sfn;
}

void PullStreamTask::ftAgree(const QString&, const Jid&, const QString& saveFileName)
{
	sfn = saveFileName;
	bool ok = false;
	for (int i = 0; i < pr.count(); i++)
	{
		printf("Protocol : %s\n", pr[i].toLatin1().constData());
		if (pr[i] == XMLNS_BYTESTREAMS)
			ok = true;
	}

	if (!ok)
	{
		Stanza stanza(Stanza::IQ, "error", id, f.full());
		QDomNode node = stanza.node().firstChild();
		QDomDocument doc("");
	
		QDomElement error = doc.createElement("error");
		error.setAttribute("code", 400);
		error.setAttribute("type", "cancel");

		QDomElement badrequest = doc.createElement("bad-request");
		badrequest.setAttribute("xmlns", "urn:ietf:params:xml:ns:xmpp-stanzas");

		QDomElement novalidstreams = doc.createElement("no-valid-streams");
		novalidstreams.setAttribute("xmlns", XMLNS_SI);

		error.appendChild(novalidstreams);
		error.appendChild(badrequest);
		node.appendChild(error);
	
		p->write(stanza);
		
		return;
	}
	
	Stanza stanza(Stanza::IQ, "result", id, f.full());
	QDomNode node = stanza.node().firstChild();
	QDomDocument doc("");

	QDomElement si = doc.createElement("si");
	si.setAttribute("xmlns", XMLNS_SI);

	QDomElement feature = doc.createElement("feature");
	feature.setAttribute("xmlns", XMLNS_FEATURENEG);

	QDomElement x = doc.createElement("x");
	x.setAttribute("xmlns", "jabber:x:data");
	x.setAttribute("type", "submit");

	QDomElement field = doc.createElement("field");
	field.setAttribute("var", "stream-method");

	QDomElement value = doc.createElement("value");

	QDomText method = doc.createTextNode(XMLNS_BYTESTREAMS);
	value.appendChild(method);

	node.appendChild(si);
	si.appendChild(feature);
	feature.appendChild(x);
	x.appendChild(field);
	field.appendChild(value);
	
	p->write(stanza);
}

//-------------------------------
// JingleTask
//-------------------------------

JingleTask::JingleTask(Task* parent, Xmpp *xmpp)
	:Task(parent)
{
	p = xmpp;
}

JingleTask::~JingleTask()
{

}

bool JingleTask::canProcess(const Stanza&) const
{
	return false;
}

void JingleTask::processStanza(const Stanza&)
{
	
}

void JingleTask::initiate(const Jid& to)
{
	QStringList contentList;
	contentList << XMLNS_VIDEO;
	id = randomString(10);
	t = to;
	cList = contentList;
	s = randomString(10);

	Stanza stanza(Stanza::IQ, "set", id, t.full());
	QDomNode node = stanza.node().firstChild();
	QDomDocument doc("");

	QDomElement jingle = doc.createElement("jingle");
	jingle.setAttribute("xmlns", XMLNS_JINGLE);
	jingle.setAttribute("action", "session-initiate");
	jingle.setAttribute("initiator", p->node().full());
	jingle.setAttribute("sid", s);

	QDomElement content = doc.createElement("content");
	content.setAttribute("creator", "initiator");
	/*
	 * The name attribute is arbitrary.
	 */
	content.setAttribute("name", "prop");
	/*
	 * The profile here is the profile used for Audio or Video
	 * (Audio/Video Profile, AVP) through 
	 * Realtime Transfer Protocol (RTP/AVP)
	 */
	content.setAttribute("profile", "RTP/AVP");
	for (int i = 0; i < contentList.count(); i++)
	{
		QDomElement description = doc.createElement("description");
		description.setAttribute("xmlns", contentList[i]);
		/*Only Video is supported now.*/
		if (contentList[i] == XMLNS_VIDEO)
		{
			/*
			 * Only JPEG payload supported now.
			 * More to be added.
			 */
			QDomElement payload = doc.createElement("payload-type");
			/* FIXME: I don't know if there is a particular id to set for JPEG*/
			payload.setAttribute("id", "0"); 
			payload.setAttribute("name", "JPEG");
			/* 
			 * No idea of the right Vale for clockrate.
			 */
			payload.setAttribute("clockrate", "90000");
			
			/* 
			 * Maybe width and height will not be used.
			 * Remove those two lines if it is the case.
			 */
			payload.setAttribute("height", "240");
			payload.setAttribute("width",  "320");
			description.appendChild(payload);
		}
		content.appendChild(description);
	}
	QDomElement transport = doc.createElement("transport");
	transport.setAttribute("xmlns", XMLNS_RTP);
	content.appendChild(transport);
	jingle.appendChild(content);
	node.appendChild(jingle);

	p->write(stanza);
}

