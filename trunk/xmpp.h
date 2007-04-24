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
	Xmpp(QString jid);
	~Xmpp();
	int auth(QString password, QString resource);
	bool connected();
	QList<QDomElement> elems;

public slots:
	void dataReceived();
	void sendDataFromTls();
	void clearDataReceived();
	void tlsIsConnected();

signals:
	void messageReceived(/*QString message, QString from, QString to, QString type, QString id*/);
	void presenceChanged(/*QString from, QString to, QString type, QString status, QString id*/);
	void askVersion(/*QString from, QString id*/);
	void needUserName();
	/* 
	 * Etc...
	 * See http://psi-im.org/wiki/Iris_Library/ 
	 */

private:	
	int sendData(QByteArray mess);
	QByteArray readData();
	QTcpSocket *tcpSocket;
	/*
	 * See if char cannot be QString.
	 */
	QString server;
	QString username;
	int timeOut;
	bool tlsDone;
	bool saslDone;
	bool jidDone;
	bool plainMech; // if it is true, server supports PLAIN login.
	bool needBind;
	bool needSession;

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
		waitStream,
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
	bool isConnected;
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
