#ifndef XMPPREG_H
#define XMPPREG_H
#include <QObject>
#include <QTcpSocket>

#include "profile.h"
#include "xmpp.h"
#include "task.h"

/*
 * Xmpp Registration Class.
 * This manages registrations to the server.
 */

class XmppReg : public Task
{
	Q_OBJECT
public:
	XmppReg(Task* parent, Xmpp* x);
	~XmppReg();
	void registerAccount(const QString& /*username*/, const QString&/*password*/);
	void registerAccount(const Profile&);
	bool registeredOk() const {return regOk;};
	Profile profile() const;
	bool canProcess(const Stanza& s) const;
	void processStanza(const Stanza& s);

signals:
	void finished();

public slots:
	void slotRegister();
	
private:
	bool regOk;
	QTcpSocket *tcpSocket;
	Profile p;
	Xmpp *x;
	QString id;
	enum States {Start = 0, WaitResult} state;
	bool needPassword, needUsername, needEmail;
	void sendRegistration();
};

#endif //XMPPREG_H
