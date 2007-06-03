#include "ui_xmppwin.h"
#include "xmpp.h"
#include "chatwin.h"

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

private:
	Ui::xmppWin ui;
	Xmpp *client;
	QList<ChatWin*> chatWinList;
};
