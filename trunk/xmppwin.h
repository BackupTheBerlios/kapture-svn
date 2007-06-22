#ifndef XMPPWIN_H
#define XMPPWIN_H

#include "ui_xmppwin.h"
#include "xmpp.h"
#include "chatwin.h"
#include "contact.h"
#include "rosterModel.h"

class XmppWin : public QMainWindow
{
	Q_OBJECT
public:
	XmppWin();
	~XmppWin();

public slots:
	void jabberConnect();
	void jabberDisconnect();
	void clientConnected();
	void newPresence();
	void newMessage();
	void newIq();
	void sendMessage(QString to, QString message);
	void startChat(QString to);
	void error(Xmpp::ErrorType);

private:
	Ui::xmppWin ui;
	Xmpp *client;
	QList<Contact*> contactList;
	QList<Model::Nodes> nodes;
	Model *m;
};
#endif

