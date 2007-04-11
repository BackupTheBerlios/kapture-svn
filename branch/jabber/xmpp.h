#ifndef XMPP_H
#define XMPP_H

#include <QtCore>
#include <QtNetwork>

#include "xmlHandler.h"
#include "tlsHandler.h"

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
	int auth(char *password, char *ressource);
	bool connected();
	QList<QDomElement> elems;

public slots:
	void dataReceived();
	void sendDataFromTls(void *buffer, int bufSize);

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
	int sendData(QString mess);
	QByteArray readData();
	QTcpSocket *tcpSocket;
	/*
	 * See if char cannot be QString.
	 */
	QString server;
	QString username;
	int timeOut;
	bool tlsStarted;
	XmlHandler *handler;
	struct rooster
	{
		QString subscription;
		QString name;
		QString jid;
		QString ask;
	};
	enum tState 
	{
		waitStream,
		waitFeatures,
		waitStartTls,
		waitProceed,
		isHandShaking
	};
	QList<rooster> roosterList;
	bool isConnected;
	bool authenticated;
	tState state;
	TlsHandler *tls;
	QList<XmlHandler::tEvent> events;
	void doSomething(XmlHandler::tEvent elem);
	bool ready;
};

#endif //XMPP_H
