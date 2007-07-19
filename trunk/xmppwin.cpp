#include <QMessageBox>
#include <QDir>
#include <QFile>

#include "xmppwin.h"

XmppWin::XmppWin()
{
	int i = 0;
	bool found = false;
	ui.setupUi(this);
	connect(ui.jabberConnect, SIGNAL(clicked()), this, SLOT(jabberConnect()));
	connect(ui.password, SIGNAL(returnPressed()), this, SLOT(jabberConnect()));
	connect(ui.jabberDisconnect, SIGNAL(clicked()), this, SLOT(jabberDisconnect()));
	connect(ui.configBtn, SIGNAL(clicked()), this, SLOT(showConfigDial()));
	ui.jid->setText("Jid");
	ui.tableView->verticalHeader()->hide();
	ui.tableView->horizontalHeader()->hide();
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	

	// Loads predifined Profiles.
	conf = new Config();
	if (conf->noConfig)
		return;
	profilesa = conf->getProfileList();
	for (i = 0; i < profilesa.count(); i++)
	{
		/*printf("Profile : %s (%s, %s, %s, %s)\n", profilesa[i].getName().toLatin1().constData(),
							  profilesa[i].getJid().toLatin1().constData(),
							  profilesa[i].getPassword().toLatin1().constData(),
							  profilesa[i].getPersonnalServer().toLatin1().constData(),
							  profilesa[i].getPort().toLatin1().constData());
		*/
		ui.profilesComboBox->addItem(profilesa[i].getName());
	}
	ui.jid->setText(profilesa[0].getJid());
	ui.password->setText(profilesa[0].getPassword());
	ui.serverEdit->setText(profilesa[0].getPersonnalServer());
	ui.portEdit->setText(profilesa[0].getPort());
	
	connect(ui.profilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeProfile(int)));
	connected = false;
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
	/*
	 * FIXME:Maybe Client should receive a Profile instead of all data separately.
	 */
	connect(client, SIGNAL(connected()), this, SLOT(clientConnected()));
	connect(client, SIGNAL(error(Xmpp::ErrorType)), this, SLOT(error(Xmpp::ErrorType)));
	connect(client, SIGNAL(contactFeaturesReady(Xmpp::ContactFeatures)), this, SLOT(contactFeaturesSave(Xmpp::ContactFeatures)));
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
	connected = false;
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

	connect(client, SIGNAL(presence(QString, QString, QString, QString)), this, SLOT(processPresence(QString, QString, QString, QString)));
	connect(client, SIGNAL(message(QString, QString, QString)), this, SLOT(processMessage(QString, QString, QString)));
	connect(client, SIGNAL(iq(QString, QString, QString, QStringList)), this, SLOT(processIq(QString, QString, QString, QStringList)));
	
	QMessageBox::information(this, tr("Jabber"), tr("You are now connected to the server.\n You certainly will have some troubles now... :-)"), QMessageBox::Ok);
	client->getRoster();
	connected = true;
}

void XmppWin::processPresence(QString pFrom, QString pTo, QString pStatus, QString pType)
{
	int i;

	Jid *from = new Jid(pFrom);
	Jid *to = new Jid(pTo);

	// Looking for the contact in the contacts list.
	for (i = 0; i < nodes.count(); i++)
	{
		if (nodes[i].jid->equals(from))
		{
			nodes[i].presenceType = pType;
			m->setData(nodes);
			ui.tableView->update(m->index(i, 0));
			printf("Found node ! --> setting type : %s\n", pType.toLatin1().constData());
			nodes[i].jid->setResource(from->getResource());
			/* 
			 * FIXME:This could cause a lot of problems regarding multiple resources connected.
			 * The whole resource's system will be reviewd later.
			 */
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
			contactList[i]->setResource(from->getResource());
			/* 
			 * FIXME:This could cause a lot of problems regarding multiple resources connected.
			 * The whole resource's system will be reviewd later.
			 */
		}
	}
	
	/*if(!found)
	{
		Contact *contact = new Contact(from->toQString());
		contact->setPresence(pStatus, pType);
		connect(contact, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		connect(contact, SIGNAL(sendFile()), this, SLOT(sendFile()));
		contactList.append(contact);
	}*/
}

void XmppWin::processMessage(QString mFrom, QString mTo, QString mMessage)
{
	Jid *from = new Jid(mFrom);
	Jid *to = new Jid(mTo);
	
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
		Contact *contact = new Contact(from->toQString());
		contact->newMessage(mMessage);
		connect(contact, SIGNAL(sendMessage(QString, QString)), this, SLOT(sendMessage(QString, QString)));
		connect(contact, SIGNAL(sendFileSignal(QString)), this, SLOT(sendFile(QString)));
		contactList.append(contact);
	}
}

void XmppWin::processIq(QString iFrom, QString iTo, QString iId, QStringList contacts)
{
	if (iId == "roster_1")
	{
		m = new Model();
		nodes.clear();
		
		Model::Nodes node; // TODO:Nodes should be replaced by a contact object...
		QStringList l;
		for (int i = 0; i < contacts.count(); i++)
		{
			node.jid = new Jid(contacts.at(i));
			node.presenceType = "unavailable";
			nodes << node;
		}
		m->setData(nodes);
		ui.tableView->setModel(m);
		connect(ui.tableView, SIGNAL(doubleClicked(QString)), this, SLOT(startChat(QString)));
		client->setPresence();
		ui.tableView->setColumnWidth(0, 22);
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
	if (connected)
		client->sendMessage(to, message);
	else
		QMessageBox::critical(this, tr("Jabber"), tr("You are not logged in right now !!!"), QMessageBox::Ok);
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
		connect(contact, SIGNAL(sendFileSignal(QString)), this, SLOT(sendFile(QString)));
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

void XmppWin::showConfigDial()
{
	XmppConfigDialog *dial = new XmppConfigDialog();
	dial->show();
	connect(dial, SIGNAL(accepted()), this, SLOT(updateProfileList()));
}

void XmppWin::changeProfile(int p)
{
	/*
	 * FIXME:Maybe should disconnect first...
	 */
	ui.jid->setText(profilesa[p].getJid());
	ui.password->setText(profilesa[p].getPassword());
	ui.serverEdit->setText(profilesa[p].getPersonnalServer());
	ui.portEdit->setText(profilesa[p].getPort());
}

void XmppWin::updateProfileList()
{
	// Loads predifined Profiles.
	delete conf;
	disconnect(ui.profilesComboBox, SIGNAL(currentIndexChanged(int)), 0, 0);
	ui.profilesComboBox->clear();
	connect(ui.profilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeProfile(int)));
	conf = new Config();
	if (conf->noConfig)
		return;
	profilesa = conf->getProfileList();
	for (int i = 0; i < profilesa.count(); i++)
	{
		/*printf("Profile : %s (%s, %s, %s, %s)\n", profilesa[i].getName().toLatin1().constData(),
							  profilesa[i].getJid().toLatin1().constData(),
							  profilesa[i].getPassword().toLatin1().constData(),
							  profilesa[i].getPersonnalServer().toLatin1().constData(),
							  profilesa[i].getPort().toLatin1().constData());
		*/
		ui.profilesComboBox->addItem(profilesa[i].getName());
	}
	ui.jid->setText(profilesa[0].getJid());
	ui.password->setText(profilesa[0].getPassword());
	ui.serverEdit->setText(profilesa[0].getPersonnalServer());
	ui.portEdit->setText(profilesa[0].getPort());
}

void XmppWin::sendFile(QString to)
{
	client->askDiscoInfo(to, "askinfo1");
}

void XmppWin::contactFeaturesSave(Xmpp::ContactFeatures c)
{
	printf("Received features for contact %s :\n", c.jid->toQString().toLatin1().constData());
	for (int i = 0; i < c.features.count(); i++)
	{
		printf(" * %s\n", c.features[i].toLatin1().constData());
		//contactList[i].setFeatures(c);
	}
}
