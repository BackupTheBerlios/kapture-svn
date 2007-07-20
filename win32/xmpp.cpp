#include <QtXml>
#include <QDomDocument>

#include "xmpp.h"


/*
 * Establish connection to the server.
 */
Xmpp::Xmpp(QString jid, QString pServer, QString pPort)
{
	timeOut = 5000; // Default timeout set to 5 seconds
	tcpSocket = new QTcpSocket();
	sslSocket = new QSslSocket();
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connexionError(QAbstractSocket::SocketError)));
	connect(sslSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(connexionError(QAbstractSocket::SocketError)));
	username = jid.split('@').at(0);
	server = jid.split('@').at(1);

	if (pServer == "")
	{
		usePersonnalServer = false;
	}
	else
	{
		usePersonnalServer = true;
		personnalServer = pServer;
	}

	port = pPort.toInt();

	handler = new XmlHandler();
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
	stanza = new Stanza();
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

		sendData(d.toString().toLatin1());
	}
	tcpSocket->close();
}

/*
 * Start the authentication process to the server.
 */
void Xmpp::auth(QString pass, QString res)
{
	if (port != 5223)
		tcpSocket->connectToHost(usePersonnalServer ? personnalServer : server, port);
	else
		sslSocket->connectToHostEncrypted(usePersonnalServer ? personnalServer : server, port);

	password = pass;
	resource = res;
}

void Xmpp::start()
{
	QString firstXml = QString("<stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' to='%1' version=\"1.0\">").arg(server);
	//printf(" * toSend = %s\n", firstXml.toLatin1().constData());
	
	connect(port != 5223 ? tcpSocket : sslSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
	state = waitStream;
	sendData(QByteArray(firstXml.toLatin1()));
}

/*
 * Send QString to the server.
 */
int Xmpp::sendData(QByteArray mess)
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

void Xmpp::processXml(QByteArray data)
{
	QXmlInputSource xmlSource;
	QXmlSimpleReader xml;
	
	printf(" * Data : %s\n", data.constData());

	if (state < active)
	{
		xmlSource.setData(data);
		xml.setContentHandler(handler);
		
		handler->setData(data);
		
		xml.parse(xmlSource);
		//	printf(" * Parsing OK (SAX).\n");
		
		events += handler->getEvents();
	
		while (!events.isEmpty())
		{
			processEvent(events.takeFirst());
		}
	}
	else
	{
		printf("Create Stanza\n");
		/*
		 * Maybe stanza should be public. So XmppWin could directly connect to Stanza.
		 * It wil avoid having a lot of signals trough Xmpp 
		 * (Xmpp -> Stanza -> Xmpp -> XmppWin -> [chatWindow]) should become
		 * (Xmpp -> Stanza -> XmppWin)
		 */
		stanza->setData(data);
	}
	data.clear();
}

void Xmpp::processEvent(XmlHandler::Event elem) // FIXME: elem -> event
{
	//printf("Elem = %s\n", elem.name.toLatin1().constData());
	switch (state)
	{
		case waitStream:
			if (elem.name == QString("stream:stream"))
			{
				printf(" * Ok, received the stream tag.\n");
				state = waitFeatures;
			}
			//else
			//	printf(" ! Didn't receive the stream ! \n");
			break;
		case waitFeatures:
			if (elem.name == QString("stream:features"))
			{
				printf(" * Ok, received the features tag.\n");
				if (!tlsDone && useTls)
					state = waitStartTls;
				else 
					if (!saslDone)
						state = waitMechanisms;
					else
						state = waitNecessary;

			}
			break;
		case waitStartTls:
			if (elem.name == QString("starttls"))
			{
				printf(" * Ok, received the starttls tag.\n");
				// Send starttls tag
				QDomDocument doc("");
				QDomElement e = doc.createElement("starttls");
				doc.appendChild(e);
				e.setAttribute(QString("xmlns"), QString("urn:ietf:params:xml:ns:xmpp-tls"));
				sendData(QByteArray(doc.toString().toLatin1()));
				
				// Next state
				state = waitProceed;
				
				/*
				 * Even if TLS isn't required, I use TLS.
				 */
			}
			if (elem.name == QString("mechanisms"))
			{
				useTls = false;
				// Must directly switch to SASL authentication 
				state = waitMechanism;
			}
			break;
		case waitProceed:
			if (elem.name == QString("proceed"))
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
		case waitMechanisms:
			if (elem.name == QString("mechanisms"))
			{
				printf(" * Ok, received the mechanisms tag.\n");
				state = waitMechanism;
			}
			break;
		case waitMechanism:
			if (elem.name == QString("mechanism"))
			{
				printf(" * Ok, received a mechanism tag.\n");
				if (elem.text == QString("PLAIN"))
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
					sendData(doc.toString().toLatin1());
					state = waitSuccess;
				}
			}
			break;
		case waitSuccess:
			if (elem.name == QString("success"))
			{
				printf(" * Ok, SASL established.\n");
				saslDone = true;
				start();
			}
			if (elem.name == QString("failure"))
			{
				printf(" ! Check Username and password.\n");
				sendData(QString("</stream:stream>").toLatin1());
			}
			break;
		case waitNecessary:
			if (elem.name == QString("bind"))
			{
				printf(" * Ok, bind needed.\n");
				needBind = true;
			}
			if (elem.name == QString("session"))
			{
				printf(" * Ok, session needed.\n");
				needSession = true;
			}
			if (needBind && elem.name == QString("stream:features") /*&& !elem.openingTag*/)
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
				
				sendData(doc.toString().toLatin1());
				
				state = waitBind;
			}
			break;
		case waitBind:
			if (elem.name == QString("bind"))
			{
				state = waitJid;
			}
			break;
		case waitSession:
			if (elem.name == QString("iq"))
			{
				if (elem.attributes.value("type") == "result")
				{
					printf(" * Connection is now active !\n");
					//emit connected;
					
					/*
					 * Presence must be sent after getting the roster
					 * so we already have the contacts to assign their presence
					 * when receiving presence stanza's wich come after
					 * setting the first presence.
					 */

					connect(stanza, SIGNAL(presenceReady()), this, SLOT(newPresence()));
					connect(stanza, SIGNAL(messageReady()), this, SLOT(newMessage()));
					connect(stanza, SIGNAL(iqReady()), this, SLOT(newIq()));
					
					state = active;
					emit connected(); 

				}
				else
				{
					if (elem.attributes.value("type") == "error")
					{
						printf(" ! An error occured ! \n");
						//emit error;
					}
				}

			}
			break;
		case waitJid:
			if (elem.name == QString("jid"))
			{
				QString u, r, s;
				if (!elem.text.isEmpty())
				{
					u = elem.text.split('@')[0]; // Username
					s = elem.text.split('@')[1].split('/')[0]; // Server
					r = elem.text.split('/')[1]; // Resource
					printf("'%s'@'%s'/'%s'\n", u.toLatin1().constData(), s.toLatin1().constData(), r.toLatin1().constData());
				}
				
				if (u == username && s == server)
				{
					printf("Jid OK !\n");
					resource = r;
					jidDone = true;
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

				sendData(doc.toString().toLatin1());

				state = waitSession;
			}
			break;
		case waitErrorType:
			if (elem.name == "internal-server-error")
			{
				printf("Login error : unkown user.\n");
				//emit error;
			}
			break;

	}
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
	//printf("Xmpp : Clear data received\n");
	processXml(tls->read());
}

void Xmpp::tlsIsConnected()
{
	tlsDone = true;
	isTlsing = false;
	state = waitStream;
	// now that TLS is done, I relaunch the auth process.
	QString firstXml = QString("<?xml version='1.0'?><stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' to='%1' version='1.0'>").arg(server);
	//printf(" * toSend = %s\n", firstXml.toLatin1().constData()); 
	sendData(QByteArray(firstXml.toLatin1()));
}

void Xmpp::getRoster()
{
	QDomDocument d("");
	QDomElement e = d.createElement("iq");
	e.setAttribute("from", username + "@" + server + "/" + resource);
	e.setAttribute("type", "get");
	e.setAttribute("id", "roster_1");
	QDomElement q = d.createElement("query");
	q.setAttribute("xmlns", "jabber:iq:roster");
	e.appendChild(q);
	d.appendChild(e);

	sendData(d.toString().toLatin1());
}

void Xmpp::setPresence(QString show, QString status)
{
	if (show == "" && status == "")
	{
		// Send initial presence.
		QDomDocument doc("");
		QDomElement e = doc.createElement("presence");

		doc.appendChild(e);

		sendData(doc.toString().toLatin1());

	}

}

void Xmpp::sendMessage(QString to, QString message)
{
	printf("Send message from Xmpp\n");
	QDomDocument d("");
	QDomElement e = d.createElement("message");
	e.setAttribute("from", username + "@" + server + "/" + resource);
	e.setAttribute("to", to);
	e.setAttribute("type", "chat"); // The only one supported for now.
	QDomElement b = d.createElement("body");
	QDomText t = d.createTextNode(message);

	b.appendChild(t);
	e.appendChild(b);
	d.appendChild(e);

	sendData(d.toString().toLatin1());
}

void Xmpp::sendFile(QString to, unsigned int size, QString name, QString description, QDateTime date, QString hash)
{

	/*
	 * Stream Initiation
	 *  - Discovers if Receiver implements the desired profile.
	 *  - Offers a stream initiation.
	 *  - Receiver accepts stream initiation.
	 *  - Sender and receiver prepare for using negotiated profile and stream.
	 *  See XEP 0095 : http://www.xmpp.org/extensions/xep-0095.html
	 *
	 *  Should have new states for the Xmpp::processEvent(...) function.
	 */

	QDomDocument d("");
	QDomElement iq = d.createElement("iq");
	iq.setAttribute("to", to);
	iq.setAttribute("type", "set");
	iq.setAttribute("id", "transfer1");

	QDomElement file = d.createElement("file");
	file.setAttribute("size", QString("%1").arg(size));
	file.setAttribute("name", name);
	
	if(description != "")
	{
		QDomElement desc = d.createElement("desc");
		QDomText text = d.createTextNode(description);
		desc.appendChild(text);
		file.appendChild(desc);
	}
	/*
	 * Ranged transfers are not supported (yet)
	 */
	iq.appendChild(file);
	d.appendChild(iq);

	sendData(d.toString().toLatin1());
}

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

void Xmpp::sendDiscoInfo(QString to, QString id)
{
/*
 * <feature var='http://jabber.org/protocol/si'/>
 * <feature var='http://jabber.org/protocol/si/profile/file-transfer'/>
 */
	QDomDocument d("");
	QDomElement iq = d.createElement("iq");
	iq.setAttribute("type", "result");
	iq.setAttribute("from", username + '@' + server + '/' + resource);
	iq.setAttribute("to", to);
	iq.setAttribute("id", id);
	QDomElement query = d.createElement("query");
	query.setAttribute("xmlns", XMLNS_DISCO);

	/*QDomElement feature = d.createElement("feature");
	feature.setAttribute("var", "http://jabber.org/protocol/si");
	query.appendChild(feature);

	QDomElement feature2 = d.createElement("feature");
	feature2.setAttribute("var", "http://jabber.org/protocol/si/profile/file-transfer");
	query.appendChild(feature2);*/
	
	iq.appendChild(query);
	d.appendChild(iq);

	sendData(d.toString().toLatin1());
}

void Xmpp::askDiscoInfo(QString to, QString id)
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

	sendData(d.toString().toLatin1());
}

void Xmpp::newPresence()
{
	QString pFrom = stanza->getFrom();
	QString pTo = stanza->getTo();
	QString pStatus = stanza->getStatus();
	QString pType = stanza->getType();

	emit presence(pFrom, pTo, pStatus, pType);
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
			emit iq(iFrom, iTo, iId, contacts);
	}
}
