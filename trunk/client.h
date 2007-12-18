#ifndef CLIENT_H
#define CLIENT_H
#include <QDomDocument>

#include "xmpp.h"
#include "task.h"
#include "tasks.h"
#include "roster.h"
#include "incomingfiledialog.h"

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
	void setInitialPresence(QString& show, QString& status, QString&);
	void sendFile(QString&);
	Stanza *getFirstStanza();
	Task *rootTask() const;
	enum streamErrorType {
			Unknown = 1,
			Declined
			};
	void sendVideo(const QString&);
	void setPresence(const QString& show, const QString& status);

public slots:
	void read();
	void rosterFinished();
	void setPresenceFinished();
	void presenceFinished();
	void messageFinished();
	void authFinished();
	void slotInfoDone();
	void transferFinished();
	void transferFile();
	void streamTaskError(int, const QString&);
	void notConnected();
	void fileTransferIncoming();
	void ftAgree();
	void ftDecline();
	void receiveFileReady();
	void connectionError(Xmpp::ErrorType);

signals:
	/*!
	 * This signal is emitted when the stream is authenticated.
	 */
	void connected();
	void rosterReady(Roster);
	void presenceReady(const Presence&);
	void messageReady(const Message&);
	void error(Xmpp::ErrorType);
	void prcentChanged(Jid&, QString&, int); 
	void streamError(streamErrorType);

private:
	Client();
	
	IncomingFileDialog *ifd;

	Jid j;
	QString r;  	// Resource
	QString pS; 	// Personnam Server
	int p;      	// Port
	QString pass;	// Password
	QFile *f;
	Xmpp *xmpp;
	Task *task;
	RosterTask *rTask; 		//rosterTask
	PresenceTask *pTask;		//presenceTask
	MessageTask *mTask;		//messageTask
	PullPresenceTask *ppTask;	//pullPresenceTask
	PullMessageTask *pmTask;	//pullMessageTask
	PullStreamTask *psTask;		//pullStreamTask
	StreamTask *sTask;		//streamTask
	FileTransferTask *sfTask;	//sendFileTask
	FileTransferTask *rfTask;	//receiveFileTask
	JingleTask *svTask; 		//sendVideoTask
	QString fileName;
};
#endif
