#ifndef XMPPWIN_H
#define XMPPWIN_H

#include "ui_xmppwin.h"
#include "client.h"
#include "chatwin.h"
#include "contact.h"
#include "rosterModel.h"
#include "jid.h"
#include "profile.h"
#include "config.h"
#include "roster.h"
#include "xmppconfigdialog.h"
#include "emoticons.h"

//class Message;

class XmppWin : public QMainWindow
{
	Q_OBJECT
public:
	XmppWin();
	~XmppWin();
	Jid *jid;
	void getRoster();

public slots:
	void clientAuthenticated();
	void processPresence(const Presence&);
	void processMessage(const Message&);
	void sendMessage(QString&, QString&);
	void startChat(QString &to);
	void error(Xmpp::ErrorType);
	void showConfigDial();
	void changeProfile(int p);
	void updateProfileList();
	void sendFile(QString&);
	void contactFeaturesSave(Xmpp::ContactFeatures);
	void setRoster(Roster);
	void prcentChanged(Jid&, QString&, int);
	void connectingLogo();
	void statusChanged();
	void sendVideo(QString&);

private:
	Ui::xmppWin ui;
	Client *client;
	QList<Contact*> contactList; // TODO:Should be replaced by a Roster.
	QList<Model::Nodes> nodes;
	Model *m;
	QByteArray config;
	QList<Profile> profilesa;
	Config *conf;
	Roster r;
	QTimer *waitingTimer;
	int secs;
	void sortContactList();
	void closeEvent(QCloseEvent*);
	Emoticons *emoticons;
	enum State {Online = 0, Offline} connectionStatus;
	void jabberConnect();
	void jabberDisconnect();
};
#endif

