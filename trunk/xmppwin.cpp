#include <QMessageBox>
#include <QItemDelegate>

#include "xmppwin.h"
#include "rosterModel.h"
#include "chatwin.h"

XmppWin::XmppWin()
{
	ui.setupUi(this);
	connect(ui.jabberConnect, SIGNAL(clicked()), this, SLOT(jabberConnect()));
	connect(ui.jabberDisconnect, SIGNAL(clicked()), this, SLOT(jabberDisconnect()));
	ui.jid->setText("linux@localhost");
	//connect(client, SIGNAL(messageReceived()), this, SLOT(messageReceived()));
}

XmppWin::~XmppWin()
{

}

void XmppWin::jabberConnect()
{
	QString fullJid = ui.jid->text();
	QString jid;
	QString resource;
	int c = 0;
	c = fullJid.split('/').count();
	if (c < 1 || c > 2)
	{
		printf("Invalid jid !\n");
		return;
	}
	if (c == 1)
	{
		jid = fullJid;
		resource = "Kapture";
	}
	if (c == 2)
	{
		jid = fullJid.split('/').at(0);
		resource = fullJid.split('/').at(1);
	}
	
	client = new Xmpp(jid, ui.serverEdit->text(), ui.portEdit->text());
	connect(client, SIGNAL(connected()), this, SLOT(clientConnected()));
	// Must also manage error signals....
	client->auth(ui.password->text(), resource);
}

void XmppWin::jabberDisconnect()
{
	delete client;
}

void XmppWin::clientConnected()
{
	ui.jabberConnect->setEnabled(false); // FIXME: could be a "Disconnect" button.

	connect(client->stanza, SIGNAL(presenceReady()), this, SLOT(newPresence()));
	connect(client->stanza, SIGNAL(messageReady()), this, SLOT(newMessage()));
	connect(client->stanza, SIGNAL(iqReady()), this, SLOT(newIq()));
	
	QMessageBox::information(this, tr("Jabber"), tr("You are now connected to the server.\n You certainly will have some troubles now... :-)"), QMessageBox::Ok);
	client->getRoster();
}

void XmppWin::newPresence()
{
	
}

void XmppWin::newMessage()
{
	QString mFrom = client->stanza->getFrom();
	QString mTo = client->stanza->getTo();
	QString mMessage = client->stanza->getMessage();
	
	bool found = false;
	for (int i = 0; i < chatWinList.count(); i++)
	{
		if (chatWinList.at(i)->contactNode() == mFrom.split('/').at(0))
		{
			found = true;
			chatWinList.at(i)->setContactResource(mFrom.split('/').at(1));
			chatWinList.at(i)->ui.discutionText->insertHtml(QString("<font color='red'>%1 says :</font><br>%2<br>").arg(mFrom).arg(mMessage));
			chatWinList.at(i)->show();
		}
	}
	
	if (!found)
	{
		ChatWin *cw = new ChatWin();
		cw->setContactNode(mFrom);
		cw->ui.discutionText->insertHtml(QString("<font color='red'>%1 says :</font><br>%2<br>").arg(mFrom).arg(mMessage));
		connect(cw, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		cw->show();
		chatWinList.append(cw);
	}
}

void XmppWin::newIq()
{
	if (client->stanza->getId() == "roster_1")
	{
		Model *m = new Model();
		m->setData(client->stanza->getContacts());
		ui.tableView->setModel(m);
		connect(ui.tableView, SIGNAL(doubleClicked(QString)), this, SLOT(startChat(QString)));
		client->setPresence();
	}
}

void XmppWin::sendMessage(QString to, QString message)
{
	client->sendMessage(to, message);
}

void XmppWin::startChat(QString to)
{
	// Start Chat with "to" if it isn't done yet.
	// printf("Start Chat with \"to\" if it isn't done yet.\n");
	
	bool found = false;
	for (int i = 0; i < chatWinList.count(); i++)
	{
		if (chatWinList.at(i)->contactNode() == to)
		{
			found = true;
		}
	}
	
	if (!found)
	{
		ChatWin *cw = new ChatWin();
		cw->setContactNode(to);
		connect(cw, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		cw->show();
		chatWinList.append(cw);
	}
}
