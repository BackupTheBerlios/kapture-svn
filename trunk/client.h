#ifndef CLIENT_H
#define CLIENT_H
#include <QDomDocument>

#include "xmpp.h"
#include "task.h"
#include "tasks.h"
#include "roster.h"

class Client : public QObject
{
	Q_OBJECT
public:
	Client(Jid &jid, QString server, QString port);
	~Client();
	void authenticate();
	void setResource(const QString&);
	void setPassword(const QString&);
	void setJid(const Jid&);
	bool isSecured() const;
	void sendMessage(QString& to, QString& message);
	bool noStanza() const;
	void getRoster();
	void setInitialPresence(QString& show, QString& status);
	Stanza *getFirstStanza();
	Task *rootTask() const;

public slots:
	void read();
	void rosterFinished();
	void setPresenceFinished();
	void presenceFinished();
	void messageFinished();

signals:
	/*!
	 * This signal is emitted when the stream is authenticated.
	 */
	void connected();
	/*!
	 * This signal is emitted when a new stanza is arrived.
	 */
	void readyRead();
	void rosterReady(Roster);
	void presenceReady(const Presence&);
	void messageReady(const Message&);

private:
	Client();

	Jid j;
	QString r;  	// Resource
	QString pS; 	// Personnam Server
	int p;      	// Port
	QString pass;	// Password
	Xmpp *xmpp;
	Task *task;
	RosterTask *rTask;
	PresenceTask *pTask;
	MessageTask *mTask;
	PullPresenceTask *ppTask;
	PullMessageTask *pmTask;
	//void processIq(const QDomDocument& d);
	//void processPresence(const QDomDocument& d);
	//void processMessage(const QDomDocument& d);
};
#endif
