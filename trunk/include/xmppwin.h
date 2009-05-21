#ifndef XMPPWIN_H
#define XMPPWIN_H
#include <QPoint>
#include <QSystemTrayIcon>

#include "ui_xmppwin.h"
#include "client.h"
#include "chatwin.h"
#include "contact.h"
#include "rostermodel.h"
#include "jid.h"
#include "profile.h"
#include "config.h"
#include "roster.h"
#include "xmppconfigdialog.h"
#include "emoticons.h"
#include "rosterdelegate.h"

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
	void startChat(const Jid &to);
	void error(Xmpp::ErrorType);
	void showConfigDial();
	void changeProfile(int p);
	void updateConfig();
	void sendFile(QString&);
	void contactFeaturesSave(Xmpp::ContactFeatures);
	void setRoster(Roster);
	void prcentChanged(Jid&, QString&, int);
	void connectingLogo();
	void statusChanged();
	//void sendVideo(QString&);
	void showMenu(const Jid&, const QPoint&);
	void showvCard();
	void startChatFromMenu();
	void quitApp();
	void systrayActivated(QSystemTrayIcon::ActivationReason);
	void slotAddAuth();
	void slotRemAuth();
	void slotReqAuth();
	void addItem();
	void updateRosterItem(Contact*);
	void delItem();
	//void newJingleSessionReady();

private:
	Ui::xmppWin ui;
	Client *client;
	QList<Contact*> contactList; // TODO:Should be replaced by a Roster.
	QList<Model::Nodes> nodes;
	Model *m;
	ContactWidgetDelegate *d;
	QByteArray config;
	QList<Profile> profileList;
	Config *conf;
	Roster r;
	QTimer *waitingTimer;
	int secs;
	void sortContactList();
	void closeEvent(QCloseEvent*);
	Emoticons *emoticons;
	enum State {Online = 0, Chat, Dnd, Away, Xa, Invisible, Offline} connectionStatus;
	void jabberConnect();
	void jabberDisconnect();
	Jid menuTo;
	Jid pJid;
	QString password;
	QString serverEdit;
	QString portEdit;
	QString res;
	Contact* contactWithJid(const Jid&);
	QString firstShow;
	void setPresence();
	QGraphicsScene *gScene;
	QSystemTrayIcon *sti;
	QMenu *sysTrayMenu;
	bool useSystemTray;
	QMenu *menu;
	QMenu *mSubsMenu;
	//JingleTask *js; //FIXME: There should not be Tasks here, tasks stays in client.
};
#endif

