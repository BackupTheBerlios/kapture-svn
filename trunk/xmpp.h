#ifndef XMPP_H
#define XMPP_H

#include <QtCore>
#include <QtNetwork>

#include "xmlHandler.h"
#include "tlsHandler.h"
#include "stanza.h"

/*
 * This class manage xmpp stuff.
 * Here must be implmented stanzas, signals when incoming data or errors,
 * sending data,...
 */

class Xmpp : public QObject
{
	Q_OBJECT
public: 
	Xmpp(QString jid, QString pServer="", QString pPort="5222");
	~Xmpp();
	void auth(QString password, QString resource);
	bool connectedToServer();
	QList<QDomElement> elems;
	Stanza *stanza;
	void getRoster();
	void sendMessage(QString to, QString message);
	void setPresence(QString show = "", QString status = "");
	//void logOut();


public slots:
	void dataReceived();
	void sendDataFromTls();
	void clearDataReceived();
	void tlsIsConnected();
	void start();

signals:
	void messageReceived();
	void presenceChanged();
	void connected();

private:	
	int sendData(QByteArray mess);
	QByteArray readData();
	QTcpSocket *tcpSocket;
	QString server;
	QString username;
	int timeOut;
	bool tlsDone;
	bool saslDone;
	bool jidDone;
	bool plainMech; // if it is true, server supports PLAIN login.
	bool needBind;
	bool needSession;
	bool usePersonnalServer;
	QString personnalServer;
	int port;

	XmlHandler *handler;
	struct rooster
	{
		QString subscription;
		QString name;
		QString jid;
		QString ask;
	};
	enum State 
	{
		waitStream = 0,
		waitFeatures,
		waitStartTls,
		waitProceed,
		isHandShaking,
		waitMechanisms,
		waitMechanism,
		waitSuccess,
		waitNecessary,
		waitBind,
		waitSession,
		waitJid,
		waitErrorType,
		active
	};
	QList<rooster> roosterList;
	bool isConnectedToServer;
	bool authenticated;
	State state;
	TlsHandler *tls;
	QList<XmlHandler::Event> events;
	void processEvent(XmlHandler::Event elem);
	void processXml(QByteArray);
	bool isTlsing;
	QString password;
	QString resource;
};

#endif //XMPP_H
