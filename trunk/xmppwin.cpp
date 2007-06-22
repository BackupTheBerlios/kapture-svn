#include <QMessageBox>

#include "xmppwin.h"
#include "rosterModel.h"
#include "chatwin.h"

XmppWin::XmppWin()
{
	ui.setupUi(this);
	connect(ui.jabberConnect, SIGNAL(clicked()), this, SLOT(jabberConnect()));
	connect(ui.password, SIGNAL(returnPressed()), this, SLOT(jabberConnect()));
	connect(ui.jabberDisconnect, SIGNAL(clicked()), this, SLOT(jabberDisconnect()));
	ui.jid->setText("linux@localhost");
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
	// FIXME: Jid's stuff should be managed by a Jid class.
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
	QString pStatus = client->stanza->getStatus();
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

	// Look in a ContactList for a Contact that has an open ChatWin open.
	bool found = false;
	for (int i = 0; i < contactList.count(); i++)
	{
		if (fromNode == contactList[i]->getJid())
		{
			found = true;
			contactList[i]->setPresence(/*status*/ pStatus,/*show*/ pType);
		}
	}
	
	if(!found)
	{
		Contact *contact = new Contact(fromNode);
		contact->setPresence(/*status*/ pStatus,/*show*/ pType);
		connect(contact, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		contactList.append(contact);
	}
}

void XmppWin::newMessage()
{
	QString mFrom = client->stanza->getFrom();
	QString mTo = client->stanza->getTo();
	QString mMessage = client->stanza->getMessage();
	
	bool found = false;
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->getJid() == mFrom.split('/').at(0))
		{
			found = true;
			contactList[i]->setResource(mFrom.split('/').at(1));
			contactList[i]->newMessage(mMessage);
		}
	}
	if (!found)
	{
		Contact *contact = new Contact(mFrom.split('/').at(0));
		contact->newMessage(mMessage);
		connect(contact, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		contactList.append(contact);
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
	/* Still a lot to implement.
	 * Next one : File Transfert, See http://www.xmpp.org/extensions/xep-0096.html (XEP 0096 : File Transfert)
	 * Wish : Jingle support : Video Over IP, See http://www.xmpp.org/extensions/xep-0166.html
	 * 					  and http://www.xmpp.org/extensions/xep-0180.html
	 * 					   or http://www.xmpp.org/extensions/xep-0181.html
	 */
}

void XmppWin::sendMessage(QString to, QString message)
{
	client->sendMessage(to, message);
}

void XmppWin::startChat(QString to)
{
	// Start Chat with "to" if it isn't done yet.
	
	bool found = false;
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->getJid() == to)
		{
			contactList[i]->startChat();
			found = true;
		}
	}
	
	if (!found)
	{
		Contact *contact = new Contact(to);
		connect(contact, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		contact->startChat();
		contactList.append(contact);
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

