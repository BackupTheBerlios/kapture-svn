#ifndef TASKS_H
#define TASKS_H

#include <QFile>

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
	RosterTask(Task* parent = 0);
	~RosterTask();
	void getRoster(Xmpp* p, Jid& j);
	bool canProcess(const Stanza& s) const;
	void processStanza(const Stanza& s);
	Roster roster() const;
private:
	Roster r;
	QString id;
};



class PresenceTask : public Task
{
public:
	PresenceTask(Task* parent = 0);
	~PresenceTask();
	void setPresence(Xmpp* p, const QString& show, const QString& status, const QString& type); //There should be a Status class.
};



class PullPresenceTask : public Task
{
	Q_OBJECT
public:
	PullPresenceTask(Task* parent);
	~PullPresenceTask();
	bool canProcess(const Stanza& s) const;
	void processStanza(const Stanza& s);
	Presence getPresence() const;

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
	//QFile file() const;

signals:
	void infoDone();

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
		WaitAcceptFileTransfer
	} state;
	QString id;
	QStringList featureList;
	Xmpp *p;
	Jid to;
	QString profileToUse;
	QString SID; //id

};

class FileTransferTask : public Task
{
	Q_OBJECT
public:
	FileTransferTask(Task *parent, const Jid& t, Xmpp *xmpp);
	~FileTransferTask();
	void start(const QString&, const QString&, const QString&);
	bool canProcess(const Stanza&) const;
	void processStanza(const Stanza&);

public slots:
	void newConnection();
	void dataAvailable();
	void readS5();
	void writeNext(qint64);

private:
	void startByteStream(const QString&);
	QString id;
	Jid to;
	Xmpp *p;
	QTcpServer *test;
	QTcpSocket *socks5Socket;
	Socks5 *socks5;
	QString s;
	QFile *f;
	qint64 writtenData;

};
#endif
