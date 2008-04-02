#ifndef TASKS_H
#define TASKS_H

#include <QFile>
#include <QTimer>

#include "task.h"
#include "xmpp.h"
#include "jid.h"
#include "roster.h"
#include "presence.h"
#include "message.h"
#include "stanza.h"
#include "socks5.h"

class RosterTask : public Task
{
public:
	RosterTask(Xmpp* p, Task* parent = 0);
	~RosterTask();
	void getRoster(const Jid& j);
	bool canProcess(const Stanza& s) const;
	void processStanza(const Stanza& s);
	Roster roster() const;
	void addItem(const Jid& jid, const QString& name);
	void delItem(const Jid& jid);
private:
	Roster r;
	QString id;
	Xmpp *p;
	bool addContact;
};



class PresenceTask : public Task
{
	Q_OBJECT
public:
	PresenceTask(Xmpp *xmpp, Task* parent = 0);
	~PresenceTask();
	void setPresence(const QString& show, const QString& status, const QString& type); //FIXME:There should be a Status class.
	void setSubscription(const Jid& to, const QString&  type);
	bool canProcess(const Stanza& s) const;
	void processStanza(const Stanza& s);

signals:
	void subApproved();
	void subRefused();

private:
	bool waitSub;
	Xmpp *p;
};



class PullPresenceTask : public Task
{
	Q_OBJECT
public:
	PullPresenceTask(Task* parent);
	~PullPresenceTask();
	bool canProcess(const Stanza& s) const;
	void processStanza(const Stanza& s);
	Presence getPresence();

signals:
	void presenceFinished();

private:
	Jid from;
	QString type;
	QString show;
	QString status;
};

class PullMessageTask : public Task
{
	Q_OBJECT
public:
	PullMessageTask(Task* parent);
	~PullMessageTask();
	bool canProcess(const Stanza&) const;
	void processStanza(const Stanza&);
	Message getMessage();

signals:
	void messageFinished();
	
private:
	QString m;   	// Message
	Jid f;       	// From (Sender)
	Jid t;		// To (Receiver)
	QString ty;   	// Type (chat, ...)
	QString sub; 	// Subject (Not used yet)
	QString thr;  	// Thread (Not used yet)
};

class MessageTask : Task
{
public:
	MessageTask(Task* parent);
	~MessageTask();
	void sendMessage(Xmpp* p, const Message& message);
};

class StreamTask : public Task
{
	Q_OBJECT
public:
	StreamTask(Task* parent, Xmpp *xmpp, const Jid& t);
	~StreamTask();
	void initStream(const QFile&);
	void discoInfo();
	bool canProcess(const Stanza&) const;
	void processStanza(const Stanza&);
	bool supports(const QString& profile);
	QString negProfile() const;
	Jid toJid() const;
	QString sid() const;
	QStringList proxies() const;
	QStringList ports() const;
	QStringList ips() const;

signals:
	void infoDone();
	void error(int, const QString&);

private:
	/*
	 * Stream Initiation
	 *  - Discovers if Receiver implements the desired profile.
	 *  - Offers a stream initiation.
	 *  - Receiver accepts stream initiation.
	 *  - Sender and receiver prepare for using negotiated profile and stream.
	 *  See XEP 0095 : http://www.xmpp.org/extensions/xep-0095.html
	 */
	QFile f;
	enum States {
		WaitDiscoInfo = 0,
		WaitAcceptFileTransfer,
		WaitProxies,
		WaitIsProxy,
		WaitProxyIp
	} state;
	QString id;
	QStringList featureList;
	QStringList itemList;
	QStringList proxyList;
	QStringList proxyList2;
	QStringList ipList;
	QStringList portList;
	QStringList zeroconfList;
	Xmpp *p;
	Jid to;
	QString profileToUse;
	QString SID; //id
	void getProxies();
	void isAProxy(QString);
	void getProxyIp(QString);

};

class PullStreamTask : public Task
{
	Q_OBJECT
public:
	PullStreamTask(Task *parent, Xmpp *xmpp);
	bool canProcess(const Stanza&) const;
	void processStanza(const Stanza&);
	Jid from() const;
	QString fileName() const;
	int fileSize() const;
	QString fileDesc() const;
	void ftDecline(const QString&, const Jid&);
	void ftAgree(const QString&, const Jid&, const QString&);
	QList<StreamHost> streamHosts() const;
	QString sid() const;
	QString lastId() const;
	QString saveFileName() const;

signals:
	void fileTransferIncoming();
	void receiveFileReady();
private:
	QString id;
	Xmpp *p;

	//Concerns File Transfer
	Jid f; //from
	QString SID;
	QStringList pr; //protocols
	QString d; // Desctription
	QString name;
	int size;
	QString hash;
	QString desc;
	QString date;
	QList<StreamHost> streamHostList;
	QString sfn; // Save File Name.
};

class FileTransferTask : public Task
{
	Q_OBJECT
public:
	FileTransferTask(Task *parent, const Jid& t, Xmpp *xmpp);
	~FileTransferTask();
	void start(const QString&, const QString&, const QString&, const QStringList, const QStringList, const QStringList);
	bool canProcess(const Stanza&) const;
	void processStanza(const Stanza&);
	void connectToHosts(QList<PullStreamTask::StreamHost>, const QString& sid, const QString& id, const QString& saveTo);
	void setFileInfo(const QString& fileName, int fileSize);

public slots:
	void noConnection();
	void newConnection();
	void dataAvailable();
	void readS5();
	void bytesWrittenSlot(qint64);
	void connectedToProxy();
	void notifyStart();
	void s5Connected();
	void receptionNotify();
	void s5Error(QAbstractSocket::SocketError);
signals:
	void prcentChanged(Jid&, QString&, int);
	void notConnected();

private:
	QList<PullStreamTask::StreamHost> h;
	QList<QTcpServer*> serverList;
	QStringList proxies;
	QStringList ips;
	QStringList ports;
	QTcpSocket *socks5Socket;
	QString usedProxy;
	QString usedIP;
	QString usedPort;
	QString st; //Save to
	QString fileName; // Ouch !
	QString filename; // Ouch !
	QString s; //SID
	QString id;
	Jid usedJid;
	QTimer *timeOut;
	qint64 writtenData;
	Socks5 *socks5;
	QFile *f;
	QFile *fileOut;
	Xmpp *p;
	Jid to;
	bool connectToProxy;
	bool isRecept; // Tells if we are receiving (true) or sending (false) a file.
	bool fileOpened;
	void startByteStream();
	void cancel();
	void tryToConnect(PullStreamTask::StreamHost host);
	int prc, prc2, filesize;
};

/*-----------------------------------------------
 * JingleTask
 * ---------------------------------------------
 */

class JingleTask : public Task
{
	Q_OBJECT
public:
	JingleTask(Task* parent, Xmpp* xmpp);
	~JingleTask();
	bool canProcess(const Stanza&) const;
	void processStanza(const Stanza&);
	void initiate(const Jid& to);
	void setData(const QString& SID,
		     const Jid& to,
		     const QString& pId,
		     const QString& pName,
		     const QString& pCR,
		     const QString& tPort,
		     const QString& tIp,
		     const QString& tGen);
	void startReceive();
	void decline();
	Jid to() const {return t;};

public slots:
	void newConnection();
	void dataRead();

private:
	Xmpp *p;
	QString id;
	QString sID; // SID
	Jid t; // to
	QStringList cList; // contentList
	enum States {WaitData = 0, Streaming} state;
	QTcpServer *tcpServer;
	QTcpSocket *tcpStream;
};

class PullJingleTask : public Task
{
	Q_OBJECT
public:
	PullJingleTask(Task* parent, Xmpp* xmpp);
	~PullJingleTask();
	bool canProcess(const Stanza&) const;
	void processStanza(const Stanza&);
	JingleTask *getNextSession();
	bool hasPendingSession();
signals:
	void newSession();
private:
	Xmpp *p;
	QString id;
	Jid to;
	void tryToConnect();
	QString sID, tPort, tIp, tGen, pId, pName, pCR;
	QTcpServer *tcpServer;
	JingleTask *session;
	QList<JingleTask*> sessionList;
	Task *task;
};

#endif
