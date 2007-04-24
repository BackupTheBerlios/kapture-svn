#include <QtXml>
#include <QDomDocument>
//#include <openssl/ssl.h>
//#include <qca.h>

#include "xmpp.h"
//#include "xmlHandler.h"


/*
 * Establish connection to the server.
 */
Xmpp::Xmpp(QString jid)
{
	timeOut = 30000; // Default timeout set to 5 seconds
	tcpSocket = new QTcpSocket();
	username = jid.split('@').at(0);
	server = jid.split('@').at(1);

	tcpSocket->connectToHost(server, 5222);
	if (tcpSocket->waitForConnected(timeOut))
	{
		printf(" * Connected to %s!\n", server.toLatin1().constData());
		isConnected = true;
	}
	else
	{
		printf(" * Not connected ! (Unable to connect to %s)\n", server.toLatin1().constData());
		isConnected = false;
	}
	authenticated = false;
	handler = new XmlHandler();
	isTlsing = false;
	tlsDone = false;
	saslDone = false;
	needBind = false;
	needSession = false;
	jidDone = false;
}

/*
 * Closes the connection to the server.
 */
Xmpp::~Xmpp()
{
	tcpSocket->close();
}

/*
 * Authenticates the user to the server.
 */
int Xmpp::auth(QString pass, QString res)
{
	password = pass;
	resource = res;
	QString firstXml = QString("<stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' to='%1' version=\"1.0\">").arg(server);
	//printf(" * toSend = %s\n", firstXml.toLatin1().constData()); 
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
	state = waitStream;
	sendData(QByteArray(firstXml.toLatin1()));

	return 0;
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
		tcpSocket->write(mess);
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
	if (tcpSocket->waitForReadyRead(timeOut))
		return tcpSocket->readAll();
	else
		return QByteArray();
}

bool Xmpp::connected()
{
	return isConnected;
}

void Xmpp::dataReceived()
{
	QByteArray data;
	QString mess;
	
	//printf("\n * Data avaible !\n");
	data = tcpSocket->readAll();

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
	data.clear();
}

void Xmpp::processEvent(XmlHandler::Event elem) // FIXME: elem -> event
{
	printf("Elem = %s\n", elem.name.toLatin1().constData());
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
				if (!tlsDone)
					state = waitStartTls;
				else 
					if (!saslDone)
						state = waitMechanisms;
					else
						state = waitNecessary;

			}
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
			else
			{
				//printf(" ! No STARTTLS.\n");
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
					
					// Send auth method.
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
				auth(password, resource);
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
				//Stanza *st = new Stanza(elem.node);
				if (elem.attributes.value("type") == "result")
				{
					printf(" * Connection is now active !\n");
					//emit connected;
					QDomDocument doc("");
					QDomElement e = doc.createElement("presence");
					
					doc.appendChild(e);

					sendData(doc.toString().toLatin1());
					
					state = active;

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
				
				if (u == username && s == server && r == resource)
				{
					printf("Jid OK !\n");
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
	tcpSocket->write(tls->readOutgoing());
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
