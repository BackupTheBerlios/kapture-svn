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

/*	
	QString mess;
	QByteArray data;
	QXmlInputSource xmlSource;
	QXmlSimpleReader xml;
	xmlHandler *handler = new xmlHandler();
			<stream:stream xmlns:stream="http://etherx.jabber.org/fhfjfhgfjfh" xmlns:xmlns="http://etherx.jabber.org/streams" version="1.0" to="jabber.org" />	
	mess = QString("<stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' to='%1' version=\"1.0\">").arg(server);
	sendData(mess);
	data = readData();
	printf(" * Answer 1 : %s\n", data.constData());
	
	// Analyse the answer.	
	handler->setData(data);
	xmlSource.setData(data);
	xml.setContentHandler(handler);
	if (xml.parse(xmlSource))
		printf(" * Parsing OK (SAX).\n");
	
	//emit needUserName();*/
	/*
	 * After this signal is emitted, a call to continueLoginAfterUserName()
	 * MUST be done after setting the correct username.
	 * It is the same as in Iris Library.
	 * 
	 * Maybe that's not very usefull here but it can be usefull to inform
	 * about errors that occurs during the connection or inform about the
	 * state of the client when something has happened (new presence,
	 * new message,...)
	 *
	 * A state machine should be used :
	 * - I wait a stream tag
	 * 	-> received : I wait a STARTTLS
	 * 		-> received : I send a proceed (or something ;))
	 * 			-> ...
	 * 		-> not received : Error
	 * 	-> not received : Error
	 *
	 * The return 0 is needed to stop the function here.
	 */
	//return 0;
	/*
	mess = QString("<iq type='get' to='%1' id='auth_1'>\
			<query xmlns='jabber:iq:auth'>\
			<username>%2</username>\
			</query></iq>").arg(server).arg(username);

	sendData(mess);
	data = readData();
	printf(" * Answer 2 : %s\n", data.constData());
	handler->setData(data);
	xmlSource.setData(data);
	xml.setContentHandler(handler);
	if (xml.parse(xmlSource))
		printf(" * Parsing OK (SAX).\n");

	mess = QString("<iq type='set' id='auth_2' to='%1'>\
		<query xmlns='jabber:iq:auth'>\
		<username>%2</username>\
		<password>%3</password>\
		<resource>%4</resource>\
		</query>\
		</iq>").arg(server).arg(username).arg(password).arg(ressource);
	sendData(mess);
	data = readData();
	printf(" * Answer 3 : %s\n", data.constData());
	handler->setData(data);
	xmlSource.setData(data);
	xml.setContentHandler(handler);
	if (xml.parse(xmlSource))
		printf(" * Parsing OK (SAX).\n");

	authenticated = true;
	if (authenticated)
		connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(dataReceived()));
	*/
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
	
	printf("\n * Data avaible !\n");
	data = tcpSocket->readAll();
	printf(" * Data : %s\n", data.constData());
	/*
	 * I'm not using the readData function above. 
	 * See the readData function for details.
	 */
	handler->elements.clear();
	handler->setData(data);
	xmlSource.setData(data);
	xml.setContentHandler(handler);
	
	if (xml.parse(xmlSource))
		printf(" * Parsing OK (SAX).\n");
	/*elems = handler->elements;
	for(int i = 0; i < elems.count(); i++)
	{
		printf(" * %s\n", elems.at(i).toDocument().toString().toLatin1().constData());
		doSomething(elems.at(i));
	}*/
	QDomDocument Doc;
	QDomNode node;
	QList<QDomNode> nodes;
	/*Doc.setContent(data, false);
	printf(" * Value = %s\n", Doc.toString().toLatin1().constData());
	*/
	Doc = handler->getDocument();
	QDomNodeList childNodes = Doc.childNodes();
	printf(" ^ %d Child nodes.\n", childNodes.count());
	node = Doc.firstChild();
	while(!node.isNull())
	{
		printf(" * tagName of this node : %s\n", node.toElement().tagName().toLatin1().constData());
		nodes.append(node);
		/*if (!node.firstChild().isNull())
			node = node.firstChild();
		else
		*/	
		if (!node.nextSibling().isNull())
			node = node.nextSibling();
		else
			break;
	}
	printf(" * %d childs\n", nodes.count());

	/*
	 * 				==> NOTHING WORKS. <==
	 *
	 * By the way, the handler seems to be the bes solution.
	 */

	// Only the first Child is present !!!!
	for (int i = 0; i < nodes.count(); i++)
	{
		doSomething(nodes.at(i).toElement());
	}

}

void Xmpp::doSomething(QDomElement elem)
{
	switch (state)
	{
		case waitStream:
			if (elem.tagName() == QString("stream:stream"))
			{
				printf(" * Ok, received the stream tag.\n");
				state = waitFeatures;
			}
			else
				printf(" ! Didn't receive the stream ! \n");
			break;
		case waitFeatures:
			if (elem.tagName() == QString("stream:features"))
			{
				printf(" * Ok, received the features tag.\n");
				if (!tls)
					state = waitStartTls;
				//else
				//	state = waitSASLSTUFF;
			}
		case waitStartTls:
			if (elem.tagName() == QString("starttls"))
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
				printf(" ! No STARTTLS.\n");
			}
			break;
		case waitProceed:
			if (elem.tagName() == QString("proceed"))
			{
				printf(" * Ok, received the proceed tag.\n");
				printf(" * Proceeding...\n * Enabling TLS connection.\n");
				
				state = isHandShaking;
				tls = new TlsHandler(tcpSocket->socketDescriptor());
				if(tls->connect())
					state = waitStream;
				else
					sendData(QString("</stream:stream>")); //Maybe too hard for a so little fault....

			}
			else
				printf(" ! Didn't receive the proceed tag ! \n");
			
			break;
		/*case isHandShaking:
			printf("Give clear data\n");
			tls->setTlsIncoming(data);
			//tls->connect();
			break;
		*/

	}
}

