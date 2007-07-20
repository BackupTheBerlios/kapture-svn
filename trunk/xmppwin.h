#ifndef XMPPWIN_H
#define XMPPWIN_H

#include "ui_xmppwin.h"
#include "xmpp.h"
#include "chatwin.h"
#include "contact.h"
#include "rosterModel.h"
#include "jid.h"
#include "profile.h"
#include "config.h"
#include "xmppconfigdialog.h"


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
	void processPresence(QString pFrom, QString pTo, QString pStatus, QString pType, QString pNickname);
	void processMessage(QString pFrom, QString pTo, QString mMessage);
	void processIq(QString iFrom, QString iTo, QString iId, QStringList contacts, QStringList nicknames);
	void sendMessage(QString to, QString message);
	void startChat(QString to);
	void error(Xmpp::ErrorType);
	void showConfigDial();
	void changeProfile(int p);
	void updateProfileList();
	void sendFile(QString);
	void contactFeaturesSave(Xmpp::ContactFeatures);

private:
	Ui::xmppWin ui;
	Xmpp *client;
	QList<Contact*> contactList;
	QList<Model::Nodes> nodes;
	Model *m;
	QByteArray config;
	QList<Profile> profilesa;
	Config *conf;
	bool connected;
};
#endif

