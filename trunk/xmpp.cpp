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

#include <QtXml>
#include <QDomDocument>

#include "xmpp.h"


/*
 * Establish connection to the server.
 */
Xmpp::Xmpp(const Jid &jid, const QString &pServer, const int pPort)
{
	timeOut = 5000; // Default timeout set to 5 seconds
	tcpSocket = new QTcpSocket();
	sslSocket = new QSslSocket();
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connexionError(QAbstractSocket::SocketError)));
	connect(sslSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connexionError(QAbstractSocket::SocketError)));

	username = jid.node();
	server = jid.domain();

	if (pServer == "")
	{
		usePersonnalServer = false;
	}
	else
	{
		usePersonnalServer = true;
		personnalServer = pServer;
	}

	port = pPort;
	authenticated = false;
	isTlsing = false;
	tlsDone = false;
	saslDone = false;
	needBind = false;
	needSession = false;
	jidDone = false;
	useTls = true;
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(start()));
	connect(sslSocket, SIGNAL(connected()), this, SLOT(start()));
	j = jid;
	printf("JID = %s\n", jid.full().toLatin1().constData());
}

/*
 * Closes the connection to the server.
 */
Xmpp::~Xmpp()
{
	//FIXME: Do it with a stanza class.
	if (state == active)
	{
		QDomDocument d("");
		QDomElement e = d.createElement("presence");
		e.setAttribute("type", "unavaible");
		QDomElement s = d.createElement("satuts");
		QDomText t = d.createTextNode("Logged out");

		s.appendChild(t);
		e.appendChild(s);
		d.appendChild(e);
		
		QByteArray data = d.toString().toLatin1();

		sendData(data);
	}
	tcpSocket->close();
	sslSocket->close();
//	delete tcpSocket;
//	delete sslSocket;
	delete tls;
}

/*
 * Start the authentication process to the server.
 */
void Xmpp::auth(const QString &pass, const QString &res)
{
	printf("Port = %d, Server = %s, Personnal Server = %s\n", port, server.toLatin1().constData(), personnalServer.toLatin1().constData());
	if (port != 5223)
		tcpSocket->connectToHost(usePersonnalServer ? personnalServer : server, port);
	else
		sslSocket->connectToHostEncrypted(usePersonnalServer ? personnalServer : server, port);

	password = pass;
	resource = res; //TODO:Should be removed.
	j.setResource(res);
	connect(port != 5223 ? tcpSocket : sslSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
}

void Xmpp::start()
{
	if (tlsDone || !useTls)
	{
		delete xmlReader;
		delete xmlSource;
		delete xmlHandler;
	}

	xmlReader = new QXmlSimpleReader();
	xmlSource = new QXmlInputSource();
	xmlHandler = new XmlHandler();
	xmlReader->setContentHandler(xmlHandler);
	xmlSource->setData(QString(""));
	xmlReader->parse(xmlSource, true);
	
	QString firstXml = QString("<stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' to='%1' version=\"1.0\">").arg(server);
	state = waitStream;
	QByteArray data = firstXml.toLatin1();
	sendData(data);
}

/*
 * Send QString to the server.
 */
int Xmpp::sendData(QByteArray &mess)
{
	printf("SENT : %s\n", mess.constData());
	if (tlsDone)
	{
		//printf("Xmpp : Giving clear data to tlsHandler\n");
		tls->write(mess);
	}
	else
	{
		//printf(" * Sending : %s\n", mess.constData());
		port != 5223 ? tcpSocket->write(mess) : sslSocket->write(mess);
	}
	return 0;
}

/*
 * Read data if it's ready to be read and return it.
 * Return an empty QByteArray if an error occur.
 */
QByteArray Xmpp::readData()
{
	/*
	 * If the readyRead signal has been emitted, it has not to wait
	 * for ready data to read !
	 * --> dataReceived will read data itself.
	 *
	 *  PS: has dataReceived is executed each time data is ready to be
	 *  read, the readData function becomes obsolete.
	 *
	 */
	if (port != 5223)
	{
		if (tcpSocket->waitForReadyRead(timeOut))
			return tcpSocket->readAll();
		else
			return QByteArray();
	}
	else
	{
		if (sslSocket->waitForReadyRead(timeOut))
			return sslSocket->readAll();
		else
			return QByteArray();
		
	}
}

bool Xmpp::connectedToServer()
{
	return isConnectedToServer;
}

void Xmpp::dataReceived()
{
	QByteArray data;
	QString mess;
	
	data = port != 5223 ? tcpSocket->readAll() : sslSocket->readAll();
	if (data == "" || data.isNull())
		return;
	printf("\n * Data avaible !\n");

	if (state == isHandShaking || tlsDone)
	{
		/*printf(" * RECEIVED (%d bytes):\n", data.count());
		for (int i = 0; i < data.count(); i++)
			printf("0x%02hhx ", data.data()[i]);
		printf("\n");*/
		tls->writeIncoming(data);
	}
	else
		processXml(data);
}

void Xmpp::processXml(QByteArray &data)
{
	printf(" * Data : %s\n", data.constData());
/*
 * FIXME:must remove all end-of-line characters.
 * If not, QDomNode has some troubles when 
 * receiving informations for file transfer.
 */
	data.replace('\n', "");
	data.replace('\r', "");

	xmlSource->setData(data);

	if (!xmlReader->parseContinue())
	{
		printf("Parsing error\n");
		return;
	}

	Event *event;
	while ((event = xmlHandler->read()) != 0)
		processEvent(event);
}

void Xmpp::processEvent(Event *event)
{
	/*
	 * WARNING: An event is NOT still the same as before.
	 * Now, an event contains all data from depth = 1 
	 * to depth back to 1.
	 */
	printf("Elem = %s\n", event->node().localName().toLatin1().constData());
	switch (state)
	{
		case waitStream:
			if (event->type() == Event::Stream)
			{
				printf(" * Ok, received the stream tag.\n");
				state = waitFeatures;
			}
			//else
			//	printf(" ! Didn't receive the stream ! \n");
			break;
		case waitFeatures:
			if (event->node().localName() == "features")
			{
				printf(" * Ok, received the features tag.\n");
				if (!tlsDone && useTls)
				{
					QDomNode node = event->node().firstChild();
					printf("Next Status : ");
					//state = waitStartTls;
					printf("%s\n", node.localName().toLatin1().constData());
					if (node.localName() == QString("mechanisms"))
					{
						printf("Must directly switch to SASL authentication\n");
						useTls = false;
						node = node.firstChild();
						// Must directly switch to SASL authentication
						printf("%s\n", node.localName().toLatin1().constData());
						while(node.localName() == QString("mechanism"))
						{
							printf(" * Ok, received a mechanism tag.\n");
							if (node.firstChild().toText().data() == QString("PLAIN"))
							{
								plainMech = true;
								printf(" * Ok, PLAIN mechanism supported\n");

								// Sstartauth method.
								QDomDocument doc("");
								QDomElement e = doc.createElement("auth");
								doc.appendChild(e);
								e.setAttribute(QString("xmlns"), QString("urn:ietf:params:xml:ns:xmpp-sasl"));
								e.setAttribute(QString("mechanism"), QString("PLAIN"));
								QString text = QString("%1%2%3%4").arg('\0').arg(username).arg('\0').arg(password);
								QDomText t = doc.createTextNode(text.toLatin1().toBase64());
								e.appendChild(t);
								QByteArray sData = doc.toString().toLatin1();
								sendData(sData);
								state = waitSuccess;
								
							}
							node = node.nextSibling();
						}
					
					}
					if (node.localName() == QString("starttls"))
					{
						printf(" * Ok, received the starttls tag.\n");
						// Send starttls tag
						QDomDocument doc("");
						QDomElement e = doc.createElement("starttls");
						doc.appendChild(e);
						e.setAttribute(QString("xmlns"), QString("urn:ietf:params:xml:ns:xmpp-tls"));
						QByteArray sData = doc.toString().toLatin1();
						sendData(sData);
						// Next state
						state = waitProceed;
						// Even if TLS isn't required, I use TLS.
					}
				}
				else
				{
					if (!saslDone)
					{
						//TODO:Must first check that event->node().firstChild() == mechanisms
						QDomNode node = event->node().firstChild().firstChild();
						printf("Tls done or not used. --> sasl\n");
						while(node.localName() == QString("mechanism"))
						{
							printf(" * Ok, received a mechanism tag.\n");
							if (node.firstChild().toText().data() == QString("PLAIN"))
							{
								plainMech = true;
								printf(" * Ok, PLAIN mechanism supported\n");

								// Sstartauth method.
								QDomDocument doc("");
								QDomElement e = doc.createElement("auth");
								doc.appendChild(e);
								e.setAttribute(QString("xmlns"), QString("urn:ietf:params:xml:ns:xmpp-sasl"));
								e.setAttribute(QString("mechanism"), QString("PLAIN"));
								QString text = QString("%1%2%3%4").arg('\0').arg(username).arg('\0').arg(password);
								QDomText t = doc.createTextNode(text.toLatin1().toBase64());
								e.appendChild(t);
								QByteArray sData = doc.toString().toLatin1();
								sendData(sData);
								state = waitSuccess;
								
							}
							node = node.nextSibling();
						} //FIXME: this is impemented two times in this function. Another way to do the same ?
					}
					else
					{
				//		printf("Wait Ncessary\n");
				//		state = waitNecessary;
						QDomNode node = event->node().firstChild();
						while(!node.isNull())
						{
							if (node.localName() == QString("bind"))
							{
								printf(" * Ok, bind needed.\n");
								needBind = true;
							}
							if (node.localName() == QString("session"))
							{
								printf(" * Ok, session needed.\n");
								needSession = true;
							}
							node = node.nextSibling();
						}
						if (needBind)
						{
							QDomDocument doc("");
							QDomElement e = doc.createElement("iq");
							e.setAttribute("type", "set"); // Trying without id.

							QDomElement e2 = doc.createElement("bind");
							e2.setAttribute("xmlns", "urn:ietf:params:xml:ns:xmpp-bind");
				
							QDomElement e3 = doc.createElement("resource");
							QDomText t = doc.createTextNode(resource);
				
							e3.appendChild(t);
							e2.appendChild(e3);
							e.appendChild(e2);
							doc.appendChild(e);
							QByteArray sData = doc.toString().toLatin1();
							sendData(sData);
				
							state = waitBind;
						}
					}
				}
			}
			break;
		case waitProceed:
			if (event->node().localName() == QString("proceed"))
			{
				//printf(" * Ok, received the proceed tag.\n");
				printf(" * Proceeding...\n * Enabling TLS connection.\n");
				
				state = isHandShaking;
				tls = new TlsHandler();
				connect(tls, SIGNAL(readyRead()), this, SLOT(clearDataReceived()));
				connect(tls, SIGNAL(readyReadOutgoing()), this, SLOT(sendDataFromTls()));
				connect(tls, SIGNAL(connected()), this, SLOT(tlsIsConnected()));
				tls->connect();
				state = isHandShaking;
				isTlsing = true;
			}
			break;
		case waitSuccess:
			if (event->node().localName() == QString("success"))
			{
				printf(" * Ok, SASL established.\n");
				saslDone = true;
				start();
			}
			if (event->node().localName() == QString("failure"))
			{
				printf(" ! Check Username and password.\n");
				QByteArray sData = "</stream:stream>";
				sendData(sData);
			}
			break;
		case waitBind:
			if (event->node().localName() == QString("iq"))
			{
				if (event->node().toElement().attribute("type") != QString("result"))
				{
					printf("Authentification Error.\n");
					QByteArray sData = "</stream:stream>";
					sendData(sData);
					return;
				}
				if (event->node().firstChild().localName() == QString("bind"))
				{
					QDomNode node = event->node().firstChild();
					if (node.firstChild().localName() == QString("jid"))
					{
						node = node.firstChild().firstChild();
						QString u, r, s;
						if (!node.toText().data().isEmpty())
						{
							u = node.toText().data().split('@')[0]; // Username
							s = node.toText().data().split('@')[1].split('/')[0]; // Server
							r = node.toText().data().split('/')[1]; // Resource
							printf("'%s'@'%s'/'%s'\n", u.toLatin1().constData(), s.toLatin1().constData(), r.toLatin1().constData());
						}
						if (u == username && s == server)
						{
							printf("Jid OK !\n");
							resource = r;
							jidDone = true;
							j.setResource(r);
						}
					}

					if (needSession && jidDone)
					{
						printf(" * Launching Session...\n");
						QDomDocument doc("");
						QDomElement e = doc.createElement("iq");
						e.setAttribute("to", server);
						e.setAttribute("type", "set");
						e.setAttribute("id", "sess_1");

						QDomElement e2 = doc.createElement("session");
						e2.setAttribute("xmlns", "urn:ietf:params:xml:ns:xmpp-session");

						e.appendChild(e2);
						doc.appendChild(e);

						QByteArray sData = doc.toString().toLatin1();
						sendData(sData);

						state = waitSession;
					}
					
				}
			}
			break;
		case waitSession:
			if (event->node().localName() == QString("iq"))
			{
				if (event->node().toElement().attribute("type") == "result")
				{
					printf(" * Connection is now active !\n");
					
					/*
					 * Presence must be sent after getting the roster
					 * so we already have the contacts to assign their presence
					 * when receiving presence stanza's wich come after
					 * setting the first presence.
					 */

					state = active;
					emit connected(); 

				}
				else
				{
					if (event->node().toElement().attribute("type") == "error")
					{
						printf(" ! An error occured ! \n");
					}
				}

			}
			break;
		case active:
			Stanza *s = new Stanza(event->node());
			QDomDocument doc = event->node().toDocument();
			printf("Xmpp::processEvent : node = %s\n", doc.toString().toLatin1().constData());
			stanzaList << s;
			emit readyRead();
	}
}

QString Xmpp::getResource() const
{
	return resource;
}

void Xmpp::sendDataFromTls(/*QByteArray data*/)
{
	if (port != 5223)
		tcpSocket->write(tls->readOutgoing());
	else
		sslSocket->write(tls->readOutgoing());
}

void Xmpp::clearDataReceived()
{
	QByteArray toSend = tls->read();
	processXml(toSend);
}

void Xmpp::tlsIsConnected()
{
	tlsDone = true;
	isTlsing = false;
	//state = waitStream;
	//now that TLS is done, I relaunch the auth process.
	//QString firstXml = QString("<?xml version='1.0'?><stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' to='%1' version='1.0'>").arg(server);
	//printf(" * toSend = %s\n", firstXml.toLatin1().constData()); 
	//QByteArray sData = firstXml.toLatin1();
	//sendData(sData);
	start();
}

/*void Xmpp::sendMessage(const Jid &to, const QString &message)
{
	printf("Send message from Xmpp\n");
	QDomDocument d("");
	QDomElement e = d.createElement("message");
	e.setAttribute("from", username + "@" + server + "/" + resource);
	e.setAttribute("to", to.full());
	e.setAttribute("type", "chat"); // The only one supported for now.
	QDomElement b = d.createElement("body");
	QDomText t = d.createTextNode(message);

	b.appendChild(t);
	e.appendChild(b);
	d.appendChild(e);

	QByteArray sData = d.toString().toLatin1();
}
*/
void Xmpp::connexionError(QAbstractSocket::SocketError socketError)
{
	switch (socketError)
	{
		case QAbstractSocket::HostNotFoundError:
			printf(" ! Error = Host not found !\n");
			emit error(HostNotFound);
/*
 * If there is this error, xmppwin should destroy this object and construct a new one
 * with the correct data.
 */
			break;
		case QAbstractSocket::NetworkError:
			printf(" ! Network error ! Will reconnect in 30 seconds.\n");
			emit error(NetworkIsDown);
			/*must try to reconnect itself....*/
			break;
		default:
			printf(" ! An Unknown error occured. Sorry.\n");
			emit error(UnknownError);
	}
	printf("Error code : %d\n", (int)socketError);


	/* must reinitialize data....*/
}

/*
 * Calling this function before the client is connected has
 * no sense.
 */
bool Xmpp::isSecured() const
{
	return useTls;
}

Stanza *Xmpp::getFirstStanza()
{
	return stanzaList.takeFirst();
}

void Xmpp::write(Stanza& s)
{
	QByteArray sData = s.node().toDocument().toByteArray();
	printf("Write : %s\n", sData.constData());
	sendData(sData);
}

bool Xmpp::stanzaAvailable() const
{
	return !stanzaList.isEmpty();
}

Jid Xmpp::node() const
{
	return j;
}

/*void Xmpp::sendDiscoInfo(QString &to, QString &id)
{
 *
 * <feature var='http://jabber.org/protocol/si'/>
 * <feature var='http://jabber.org/protocol/si/profile/file-transfer'/>
 *
	QDomDocument d("");
	QDomElement iq = d.createElement("iq");
	iq.setAttribute("type", "result");
	iq.setAttribute("from", username + '@' + server + '/' + resource);
	iq.setAttribute("to", to);
	iq.setAttribute("id", id);
	QDomElement query = d.createElement("query");
	query.setAttribute("xmlns", XMLNS_DISCO);

	*QDomElement feature = d.createElement("feature");
	feature.setAttribute("var", "http://jabber.org/protocol/si");
	query.appendChild(feature);

	QDomElement feature2 = d.createElement("feature");
	feature2.setAttribute("var", "http://jabber.org/protocol/si/profile/file-transfer");
	query.appendChild(feature2);*
	
	iq.appendChild(query);
	d.appendChild(iq);

	QByteArray sData = d.toString().toLatin1();
	sendData(sData);
}

void Xmpp::askDiscoInfo(QString &to, QString &id)
{
	QDomDocument d("");
	QDomElement iq = d.createElement("iq");
	iq.setAttribute("type", "get");
	iq.setAttribute("from", username + '@' + server + '/' + resource);
	iq.setAttribute("to", to);
	iq.setAttribute("id", id);
	QDomElement query = d.createElement("query");
	query.setAttribute("xmlns", XMLNS_DISCO);

	iq.appendChild(query);
	d.appendChild(iq);

	QByteArray sData = d.toString().toLatin1();
	sendData(sData);
}

void Xmpp::newPresence()
{
	QString pFrom = stanza->getFrom();
	QString pTo = stanza->getTo();
	QString pStatus = stanza->getStatus();
	QString pType = stanza->getType();
	QString pNickname = stanza->getNickname();

	emit presence(pFrom, pTo, pStatus, pType, pNickname);
}

void Xmpp::newMessage()
{
	QString mFrom = stanza->getFrom();
	QString mTo = stanza->getTo();
	QString mMessage = stanza->getMessage();
	
	emit message(mFrom, mTo, mMessage);
}

void Xmpp::newIq()
{
	ContactFeatures tmp;
	
	QString iFrom = stanza->getFrom();
	QString iTo = stanza->getTo();
	QString iId = stanza->getId();
	int action = stanza->getAction();
	QStringList contacts = stanza->getContacts();
	QStringList nicknames = stanza->getNicknameList();
	Jid *from = new Jid(iFrom);
	
	int i = 0;
	bool exists = false;

	switch (action) {
		case 0 :
			printf("Send disco Info\n");
			sendDiscoInfo(iFrom, iId);
			break;
		case 1 :
			tmp.jid = new Jid(iFrom);
			tmp.features = stanza->getFeatures();
			emit contactFeaturesReady(tmp);
			break;
		default :
			emit iq(iFrom, iTo, iId, contacts, nicknames);
	}
}
*/
