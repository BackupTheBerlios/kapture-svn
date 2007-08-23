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

class FileTransferTask : Task
{
public:
	FileTransferTask(Task* parent);
	~FileTransferTask();
	void transferFile(Xmpp*, const Jid&, const QFile&);
	bool canProcess(const Stanza&) const;
	void processStanza(const Stanza&);

private:
	/*
	 * Stream Initiation
	 *  - Discovers if Receiver implements the desired profile.
	 *  - Offers a stream initiation.
	 *  - Receiver accepts stream initiation.
	 *  - Sender and receiver prepare for using negotiated profile and stream.
	 *  See XEP 0095 : http://www.xmpp.org/extensions/xep-0095.html
	 */
	void initStream();
	QFile f;
	enum States {
		WaitDiscoInfo = 0
	} state;

};
#endif
