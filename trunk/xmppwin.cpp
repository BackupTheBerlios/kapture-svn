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
	ui.tableView->verticalHeader()->hide();
	ui.tableView->horizontalHeader()->hide();
}

XmppWin::~XmppWin()
{

}

void XmppWin::jabberConnect()
{
	ui.jabberConnect->setEnabled(false);
	ui.jabberDisconnect->setEnabled(true);
	
	jid = new Jid(ui.jid->text());
	
	if (!jid->isValid())
	{
		QMessageBox::critical(this, tr("Jabber"), tr("This is an invalid Jid."), QMessageBox::Ok);
		jabberDisconnect();
		printf("Invalid jid !\n");
		return;
	}
	
	client = new Xmpp(jid->getNode(), ui.serverEdit->text(), ui.portEdit->text());
	connect(client, SIGNAL(connected()), this, SLOT(clientConnected()));
	connect(client, SIGNAL(error(Xmpp::ErrorType)), this, SLOT(error(Xmpp::ErrorType)));
	client->auth(ui.password->text(), jid->getResource() == "" ? "Kapture" : jid->getResource());
}

void XmppWin::jabberDisconnect()
{
	ui.jabberConnect->setEnabled(true);
	ui.jabberDisconnect->setEnabled(false);
	delete client;
	for (int i = 0; i < nodes.count(); i++)
	{
		nodes[i].presenceType = "unavailable";
	}
	m->setData(nodes);
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
	//QString pFrom = client->stanza->getFrom();
	//QString pTo = client->stanza->getTo();
	QString pStatus = client->stanza->getStatus();
	QString pType = client->stanza->getType();

	Jid *from = new Jid(client->stanza->getFrom());
	Jid *to = new Jid(client->stanza->getTo());

/*	QString fromNode;
	if(pFrom.split('/').count() == 0)
		return;
	if(pFrom.split('/').count() == 1)
		fromNode = pFrom;
	if(pFrom.split('/').count() == 2)
		fromNode = pFrom.split('/').at(0);
*/
	// Looking for the contact in the contacts list.
	for (i = 0; i < nodes.count(); i++)
	{
		//printf("Lookingk for node %s (does it equals %s ?)\n", fromNode.toLatin1().constData(), nodes[i].node.toLatin1().constData());
		if (nodes[i].jid->equals(from))
		{
			nodes[i].presenceType = pType;
			m->setData(nodes);
			ui.tableView->update(m->index(i, 0));
			printf("Found node ! --> setting type : %s\n", pType.toLatin1().constData());
			break;
		}
	}

	// Look in a ContactList for a Contact that has an open ChatWin open.
	bool found = false;
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(from))
		{
			found = true;
			contactList[i]->setPresence(/*status*/ pStatus,/*show*/ pType);
		}
	}
	
	if(!found)
	{
		Contact *contact = new Contact(from->toQString());
		contact->setPresence(/*status*/ pStatus,/*show*/ pType);
		connect(contact, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		contactList.append(contact);
	}
}

void XmppWin::newMessage()
{
	//QString mFrom = client->stanza->getFrom();
	//QString mTo = client->stanza->getTo();
	Jid *from = new Jid(client->stanza->getFrom());
	Jid *to = new Jid(client->stanza->getTo());
	QString mMessage = client->stanza->getMessage();
	
	bool found = false;
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(from))
		{
			found = true;
			contactList[i]->jid->setResource(jid->getResource());
			contactList[i]->newMessage(mMessage);
		}
	}
	if (!found)
	{
		Contact *contact = new Contact(jid->toQString());
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
			node.jid = new Jid(l.at(i));
			node.presenceType = "unavailable";
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

void XmppWin::startChat(QString sTo)
{
	
	// Start Chat with "to" if it isn't done yet.
	Jid *to = new Jid(sTo);
	
	bool found = false;
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(to))
		{
			contactList[i]->startChat();
			found = true;
		}
	}
	
	if (!found)
	{
		Contact *contact = new Contact(to->toQString());
		connect(contact, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		contact->startChat();
		contactList.append(contact);
	}
	//client->sendFile("linux2@localhost/Psi", 256, "test.txt", "Testing file transfer...");
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

