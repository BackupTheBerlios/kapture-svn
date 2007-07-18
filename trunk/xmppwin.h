#ifndef XMPPWIN_H
#define XMPPWIN_H

#include "ui_xmppwin.h"
#include "xmpp.h"
#include "chatwin.h"
#include "contact.h"
#include "rosterModel.h"
#include "jid.h"
#include "profile.h"

class XmppWin : public QMainWindow
{
	Q_OBJECT
public:
	XmppWin();
	~XmppWin();
	Jid *jid;

public slots:
	void jabberConnect();
	void jabberDisconnect();
	void clientConnected();
	void processPresence(QString pFrom, QString pTo, QString pStatus, QString pType);
	void processMessage(QString pFrom, QString pTo, QString mMessage);
	void processIq(QString iFrom, QString iTo, QString iId, QStringList contacts);
	void sendMessage(QString to, QString message);
	void startChat(QString to);
	void error(Xmpp::ErrorType);
	void showConfigDial();
	void changeProfile(int p);

private:
	Ui::xmppWin ui;
	Xmpp *client;
	QList<Contact*> contactList;
	QList<Model::Nodes> nodes;
	Model *m;
	QByteArray config;
	QList<Profile> profilesa;
};
#endif

