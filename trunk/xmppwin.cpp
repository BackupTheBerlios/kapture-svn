#include <QMessageBox>

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
	ui.jabberConnect->setEnabled(false);
	ui.jabberDisconnect->setEnabled(true);
	
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
	connect(client, SIGNAL(error(Xmpp::ErrorType)), this, SLOT(error(Xmpp::ErrorType)));
	// Must also manage error signals....
	client->auth(ui.password->text(), resource);
}

void XmppWin::jabberDisconnect()
{
	ui.jabberConnect->setEnabled(true);
	ui.jabberDisconnect->setEnabled(false);
	delete client;
	//ui.tableView->reset();
	for (int i = 0; i < nodes.count(); i++)
	{
		m->setData(m->index(i, 1), "unavaible");
	}
	
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
	int i;
	
	QString pFrom = client->stanza->getFrom();
	QString pTo = client->stanza->getTo();
	QString pType = client->stanza->getType();

	for (i = 0; i < nodes.count(); i++)
	{
		if (nodes[i].node == pFrom.split('/')[0])
		{
			break;
		}
	}

	m->setData(m->index(i, 1), pType);
	ui.tableView->update(m->index(i, 1));
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
			if (!chatWinList.at(i)->isActiveWindow())
			{
				chatWinList.at(i)->activateWindow();
			}
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
		m = new Model();
		nodes.clear();
		
		Model::Nodes node;
		QStringList l;
		l = client->stanza->getContacts();
		for (int i = 0; i < l.count(); i++)
		{
			node.node = l.at(i);
			node.state = "Offline";
			nodes << node;
		}
		m->setData(nodes);
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

void XmppWin::error(Xmpp::ErrorType e)
{
	// Still a lot of errors to manage...
	// Errors from the authentification process.

	QErrorMessage *dlg = new QErrorMessage();
	switch (e)
	{
	case Xmpp::HostNotFound:
		dlg->showMessage("An error occured while connecting : \nHost not found.");
		delete client;
		break;
	default :
		dlg->showMessage("An unknown error occured while connecting.");
	}
}
