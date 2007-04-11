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
	tlsStarted = false;
	handler = new XmlHandler();
	ready = true;
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
 * TODO: should fill the roosterList in.
 */
int Xmpp::auth(char *password, char *ressource)
{
	QString firstXml = QString("<stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' to='%1' version=\"1.0\">").arg(server);
	printf(" * toSend = %s\n", firstXml.toLatin1().constData()); 
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
	state = waitStream;
	sendData(firstXml);

	return 0;
}

/*
 * Send QString to the server.
 */
int Xmpp::sendData(QString mess)
{
	printf(" * Sending : %s\n", mess.toLatin1().constData());
	return tcpSocket->write(mess.toLatin1());
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
	QXmlInputSource xmlSource;
	QXmlSimpleReader xml;
	
//	printf("\n * Data avaible !\n");
	data = tcpSocket->readAll();
//	printf(" * Data : %s\n", data.constData());
	/*
	 * I'm not using the readData function above. 
	 * See the readData function for details.
	 */

	/*
	 * Here's what I do :
	 * I first give data to the handler and it parses it.
	 * Next, I get elements from the parser.
	 * Those are tElem type (a typedef)
	 */
	
	events.clear();
	xmlSource.setData(data);
	xml.setContentHandler(handler);
	
	if (xml.parse(xmlSource))
		//printf(" * Parsing OK (SAX).\n");
	
	events = handler->getEvents();

	for (int i = 0; i < events.count(); i++)
	{
		doSomething(events.at(i));
	}

}

void Xmpp::doSomething(XmlHandler::tEvent elem) // FIXME: elem -> event
{
	switch (state)
	{
		case waitStream:
			if (elem.name == QString("stream:stream"))
			{
				printf(" * Ok, received the stream tag.\n");
				state = waitFeatures;
			}
			else
			//	printf(" ! Didn't receive the stream ! \n");
			break;
		case waitFeatures:
			if (elem.name == QString("stream:features"))
			{
				printf(" * Ok, received the features tag.\n");
				if (!tls)
					state = waitStartTls;
				//else
				//	state = waitSASLSTUFF;
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
				sendData(doc.toString());
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
				connect(tls, SIGNAL(tlsDataAvaible(void*, int)), this, SLOT(sendDataFromTls(void*, int)));
				if(tls->connect())
					state = waitStream;
				else
					sendData(QString("</stream:stream>")); //Maybe too hard for a so little fault....
			}
			else
				//printf(" ! Didn't receive the proceed tag ! \n");
			break;
		/*case isHandShaking:
			printf("Give clear data\n");
			tls->setTlsIncoming(data);
			//tls->connect();
			break;
		*/

	}
}

void Xmpp::sendDataFromTls(void *buffer, int bufSize)
{
	//sendData(tls->toSend.takeFirst());
	printf("Writing : %d chars : %s\n", bufSize, (char*)buffer);
	tcpSocket->write((const char*)buffer, bufSize);
}

