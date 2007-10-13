#ifndef XMPP_H
#define XMPP_H

#include <QtCore>
#include <QtNetwork>

#include "xmlhandler.h"
#include "event.h"
#include "tlshandler.h"
#include "stanza.h"
#include "jid.h"

/*
 * This class manage xmpp stuff.
 * Here must be implmented stanzas, signals when incoming data or errors,
 * sending data,...
 */

class Xmpp : public QObject
{
	Q_OBJECT
public: 
	Xmpp(const Jid& jid, const QString& pServer = QString(), const int port = 5222);
	Xmpp();
	~Xmpp();
	void auth(const QString& password, const QString& resource);
	bool connectedToServer();
	void getRoster();
	bool isSecured() const;
	QString getResource() const; //FIXME: --> resource()
	enum ErrorType
	{
		HostNotFound = 0,
		NetworkIsDown,
		UnknownError
	};
	
	struct ContactFeatures
	{
		Jid *jid;
		QStringList features;
	};

	/*!
	 * Gets the firs stanza and removes it from the list.
	 * This method should be called when readyRead() is emitted.
	 */
	Stanza *getFirstStanza();
	
	/*!
	 * Send the Stanza s to the server.
	 * Returns true on success, false on failure.
	 */
	void write(Stanza& s);

	/*!
	 * Returns false if there are no more stanza ready to be read left.
	 */
	bool stanzaAvailable() const;
	Jid node() const;


public slots:
	void dataReceived();
	void sendDataFromTls();
	void clearDataReceived();
	void tlsIsConnected();
	void start();
	void connectionError(QAbstractSocket::SocketError socketError);
signals:
	void messageReceived();
	void presenceChanged();
	void connected();
	void error(Xmpp::ErrorType);
	/*!
	 * This signal is emitted when a new Stanza is ready to be read.
	 */
	void readyRead();
	
	
	/*void presence(QString, QString, QString, QString, QString);
	void message(QString, QString, QString);
	void iq(QString, QString, QString, QStringList, QStringList);
	void contactFeaturesReady(Xmpp::ContactFeatures);*/

private:
	Stanza *stanza;
	int sendData(QByteArray &mess);
	QByteArray readData();
	QTcpSocket *tcpSocket;
	QSslSocket *sslSocket;
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

	QXmlSimpleReader *xmlReader;
	QXmlInputSource *xmlSource;
	XmlHandler *xmlHandler;

	struct Roster
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
		waitProceed,
		waitSuccess,
		waitBind,
		waitSession,
		isHandShaking,
		active
	};
	QList<Roster> rosterList;
	bool isConnectedToServer;
	bool authenticated;
	State state;
	TlsHandler *tls;
	void processEvent(Event *elem);
	void processXml(QByteArray&);
	bool isTlsing;
	bool useTls;
	QString password;
	QString resource;
	QList<QDomElement> elems;
	QList<Stanza*> stanzaList;
	Jid j; // Logged User's Jid.
};

#endif //XMPP_H
