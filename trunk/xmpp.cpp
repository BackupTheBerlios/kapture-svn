#include <QtXml>
#include <QDomDocument>

#include "xmpp.h"
#include "xmlHandler.h"

/*
 * Establish connection to the server.
 */
xmpp::xmpp(QString jid)
{
	timeOut = 5000; // Default timeout set to 5 seconds
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
}

/*
 * Closes the connection to the server.
 */
xmpp::~xmpp()
{
	tcpSocket->close();
}

/*
 * Authenticates the user to the server.
 * TODO: should fill the roosterList in.
 */
int xmpp::auth(char *password, char *ressource)
{
	QString mess;
	QByteArray data;
	QXmlInputSource xmlSource;
	QXmlSimpleReader xml;
	xmlHandler *handler = new xmlHandler();
	/*
	 * FIXME: Is xmlns:stream='http://etherx.jabber.org/streams' 
	 * correct even if the server isn't jabber.org ?
	 */
	mess = QString("<stream:stream xmlns:stream='http://etherx.jabber.org/streams' xmlns='jabber:client' to='%1'>").arg(server);
	sendData(mess);
	data = readData();
	printf(" * Answer 1 : %s\n", data.constData());
	
	// Analyse the answer.	
	handler->setData(data);
	xmlSource.setData(data);
	xml.setContentHandler(handler);
	if (xml.parse(xmlSource))
		printf(" * Parsing OK (SAX).\n");
		
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

	return 0;
}

/*
 * Send data to the server.
 */
int xmpp::sendData(QString mess)
{
	printf(" * Sending : %s\n", mess.toLatin1().constData());
	return tcpSocket->write(mess.toLatin1().constData(), mess.size());
}

/*
 * Read data if it's ready to be read and return it.
 * Return an empty QByteArray if an error occur.
 */
QByteArray xmpp::readData()
{
	if (tcpSocket->waitForReadyRead(timeOut))
		return tcpSocket->readAll();
	else
		return QByteArray();
}

bool xmpp::connected()
{
	return isConnected;
}
