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
	emoticons.append(Emoticon::Emoticon(":)", "smile.png"));
	emoticons.append(Emoticon::Emoticon(":-)", "smile.png"));
	// TODO: add more emoticons
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
		QMessageBox::critical(this, tr("Jabber"), tr("This is an invalid Jid."), QMessageBox::Ok);
		jabberDisconnect();
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
	client->auth(ui.password->text(), resource);
}

void XmppWin::jabberDisconnect()
{
	ui.jabberConnect->setEnabled(true);
	ui.jabberDisconnect->setEnabled(false);
	delete client;
	for (int i = 0; i < nodes.count(); i++)
	{
		m->setData(m->index(i, 1), "Offline");
	}
	ui.tlsIconLabel->setEnabled(false);
	
}

void XmppWin::clientConnected()
{
	QPixmap *pixmap;
	ui.jabberConnect->setEnabled(false); // FIXME: could become a "Disconnect" button.

	if (client->isSecured())
	{
		pixmap = new QPixmap("encrypted.png");
		ui.tlsIconLabel->setToolTip(tr("The connection with the server is encrypted."));
	}
	else
	{
		pixmap = new QPixmap("decrypted.png");
		ui.tlsIconLabel->setToolTip(tr("The connection with the server is *not* encrypted."));
	}
	ui.tlsIconLabel->setPixmap(*pixmap);
	ui.tlsIconLabel->setEnabled(true);


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

	QString fromNode;
	if(pFrom.split('/').count() == 0)
		return;
	if(pFrom.split('/').count() == 1)
		fromNode = pFrom;
	if(pFrom.split('/').count() == 2)
		fromNode = pFrom.split('/').at(0);

	// Looking for the contact in the contacts list.
	for (i = 0; i < nodes.count(); i++)
	{
		if (nodes[i].node == fromNode)
		{
			if (pType == "unavaible")
				m->setData(m->index(i, 1), "Offline");
			if (pType == "avaible")
				m->setData(m->index(i, 1), "Online");
			ui.tableView->update(m->index(i, 1));
			break;
		}
	}

	// Looking for a ChatWin with the contact.
	bool found = false;
	for (int i = 0; i < chatWinList.count(); i++)
	{
		if (chatWinList.at(i)->contactNode() == pFrom.split('/').at(0) || /*see Jid Class*/ chatWinList.at(i)->contactNode() == pFrom)
		{
			found = true;
			// Should show the status and not the type !
			chatWinList.at(i)->ui.discutionText->insertHtml(QString("<font color='green'> * %1 is now %2</font><br>").arg(pFrom).arg(pType == "avaible" ? "online" : "offline"));
		}
	}
}

void XmppWin::newMessage()
{
	QString mFrom = client->stanza->getFrom();
	QString mTo = client->stanza->getTo();
	QString mMessage = client->stanza->getMessage();
	mMessage = changeEmoticons(mMessage);
	
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
		cw->setContactNode(mFrom.split('/').at(0));
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
		
		Model::Nodes node; // TODO:Nodes should be replaced by a contact object...
		QStringList l;
		l = client->stanza->getContacts(); // TODO:Should return a QList<contact>...
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
			chatWinList.at(i)->show();
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
	switch (e)
	{
	case Xmpp::HostNotFound:
		QMessageBox::critical(this, tr("Jabber"), tr("An error occured while connecting : \nHost not found."), QMessageBox::Ok);
		ui.jabberConnect->setEnabled(true);
		ui.jabberDisconnect->setEnabled(false);
		delete client;
		break;
	default :
		QMessageBox::critical(this, tr("Jabber"), tr("An unknown error occured while connecting."), QMessageBox::Ok);
	}
}

/*This function should NOT be there*/
/*This function changes all emoticons ( :-), ;-), :'(, ... )
 * to a HTML tag for the corresponding image so it is shown.
 */

QString XmppWin::changeEmoticons(QString m)
{
	for (int i = 0; i < emoticons.count(); i++)
	{
		m.replace(emoticons[i].binette, "<img src=\"" + emoticons[i].link + "\">");
	}
	return m;
}
