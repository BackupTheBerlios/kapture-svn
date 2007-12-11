#include <QMessageBox>
#include <QDir>
#include <QFile>

#include "xmppwin.h"
#include "message.h"
#include "presence.h"

XmppWin::XmppWin()
{
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
	ui.jabberConnect->setEnabled(true);
	ui.jabberDisconnect->setEnabled(false);

	// Loads predifined Profiles.
	conf = new Config();
	if (conf->noConfig)
	{
		QMessageBox::information(this, tr("Configuration"),
			tr("It seems that it is the firs time you \
			run the Jabber part of Kapture. Let's add an account."),
			QMessageBox::Ok);
		showConfigDial();
	}
	updateProfileList();
	
	connect(ui.profilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeProfile(int)));
	connected = false;
	secs = 0;
}

XmppWin::~XmppWin()
{

}

/*!
 * Connects and authenticates the User to the server.
 */

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
	
	client = new Client(*jid, ui.serverEdit->text(), ui.portEdit->text());
	waitingTimer = new QTimer();
	waitingTimer->start(1000);
	connect(waitingTimer, SIGNAL(timeout()), this, SLOT(connectingLogo()));
	connect(client, SIGNAL(error(Xmpp::ErrorType)), this, SLOT(error(Xmpp::ErrorType)));
	connect(client, SIGNAL(connected()), this, SLOT(clientAuthenticated()));
	client->setResource(jid->resource());
	client->setPassword(ui.password->text());
	client->authenticate();
}

void XmppWin::connectingLogo()
{
	secs++;
	QString title = "Kapture -- Connecting";
	for (int i = 0; i < (secs % 5); i++)
	{
		title.append(".");
	}
	setWindowTitle(title);
}

void XmppWin::jabberDisconnect()
{
	ui.jabberConnect->setEnabled(true);
	ui.jabberDisconnect->setEnabled(false);
	delete client;
	delete jid;
	QString status = "";
	QString type = "";
	for (int i = 0; i < contactList.count(); i++)
	{
		contactList[i]->setPresence(status, type);
	}
	m->setData(contactList);
	ui.tlsIconLabel->setEnabled(false);
	connected = false;
	setWindowTitle("Kapture [Offline]");
}

void XmppWin::clientAuthenticated()
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

	client->getRoster();
	connect(client, SIGNAL(rosterReady(Roster)), this, SLOT(setRoster(Roster)));
	connect(client, SIGNAL(presenceReady(const Presence&)), this, SLOT(processPresence(const Presence&)));
	connect(client, SIGNAL(messageReady(const Message&)), this, SLOT(processMessage(const Message&)));
	connected = true;
}

void XmppWin::setRoster(Roster roster)
{
	r = roster;
	m = new Model();
	contactList.clear();
	contactList = r.contactList();
	// Connecting contacts.
	for (int i = 0; i < contactList.count(); i++)
	{
		connect(contactList[i], SIGNAL(sendMessage(QString&, QString&)), this, SLOT(sendMessage(QString&, QString&)));
		connect(contactList[i], SIGNAL(sendFileSignal(QString&)), this, SLOT(sendFile(QString&)));
	}
	sortContactList();
	m->setData(contactList);
	ui.tableView->setModel(m);
	connect(ui.tableView, SIGNAL(doubleClicked(QString&)), this, SLOT(startChat(QString&)));
	QString a = "";
	QString b = "";
	client->setInitialPresence(a, b); 
	ui.tableView->setColumnWidth(0, 22);
	ui.tableView->resizeColumnsToContents();
	waitingTimer->stop();
	delete waitingTimer;
	setWindowTitle("Kapture -- " + jid->full());
}

void XmppWin::sortContactList()
{
	bool permut = true;
	while (permut)
	{
		permut = false;
		for (int i = 0; i < contactList.count() - 1; i++)
		{
			if (!contactList[i]->isAvailable() && contactList[i + 1]->isAvailable())
			{
				contactList.swap(i, i + 1);
				permut = true;
			}
		}
	}

}

void XmppWin::processPresence(const Presence& presence)
{
	int i;
	// FIXME:There is no feedback in the chatWin of a contact when it becomes available.

	// Looking for the contact in the contactList.
	for (i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(presence.from()))
		{
			contactList[i]->jid->setResource(presence.from().resource());
			QString status = presence.status();
			QString type = presence.type();
			contactList[i]->setPresence(status, type);
			sortContactList();
			m->setData(contactList);
			for (int j = 0; j < contactList.count(); j++)
			{
				ui.tableView->update(m->index(j, 0));
				ui.tableView->update(m->index(j, 1));
			}
			//ui.tableView->repaint();
			ui.tableView->resizeColumnsToContents();
			break;
		}
	}
}

void XmppWin::processMessage(const Message& m)
{
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(m.from()))
		{
			contactList[i]->newMessage(m.message());
		}
	}
}

void XmppWin::sendMessage(QString &to, QString &message)
{
	if (connected)
		client->sendMessage(to, message);
	else
		QMessageBox::critical(this, tr("Jabber"), tr("You are not logged in right now !!!"), QMessageBox::Ok);
}

void XmppWin::startChat(QString &sTo)
{
	// Start Chat with "to" if it isn't done yet.
	Jid *to = new Jid(sTo);
	
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(*to))
		{
			contactList[i]->startChat();
		}
	}
}

void XmppWin::error(Xmpp::ErrorType e)
{
	// Still a lot of errors to manage...
	// Errors from the authentification process.
	waitingTimer->stop();
	switch (e)
	{
	case Xmpp::HostNotFound:
		QMessageBox::critical(this, tr("Jabber"), tr("An error occured while connecting : \nHost not found."), QMessageBox::Ok);
		break;
	default :
		QMessageBox::critical(this, tr("Jabber"), tr("An unknown error occured while connecting."), QMessageBox::Ok);
	}
	ui.jabberConnect->setEnabled(true);
	ui.jabberDisconnect->setEnabled(false);
	delete client;
	delete jid;
	delete waitingTimer;
	setWindowTitle("Kapture [Offline]");

}

void XmppWin::showConfigDial()
{
	XmppConfigDialog *dial = new XmppConfigDialog();
	dial->show();
	connect(dial, SIGNAL(accepted()), this, SLOT(updateProfileList()));
}

void XmppWin::changeProfile(int p)
{
	ui.jid->setText(profilesa[p].jid());
	ui.password->setText(profilesa[p].password());
	ui.serverEdit->setText(profilesa[p].personnalServer());
	ui.portEdit->setText(profilesa[p].port());
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
	
	profilesa = conf->profileList(); //FIXME: find another name fore profilesa
	
	for (int i = 0; i < profilesa.count(); i++)
	{
		ui.profilesComboBox->addItem(profilesa[i].name());
	}
	
	if (profilesa.count() > 0)
	{
		ui.jid->setText(profilesa[0].jid());
		ui.password->setText(profilesa[0].password());
		ui.serverEdit->setText(profilesa[0].personnalServer());
		ui.portEdit->setText(profilesa[0].port());
	}
}

void XmppWin::sendFile(QString &to)
{
	connect(client, SIGNAL(prcentChanged(Jid&, QString&, int)), this, SLOT(prcentChanged(Jid&, QString&, int)));
	client->sendFile(to);
}

void XmppWin::contactFeaturesSave(Xmpp::ContactFeatures c)
{
	int i = 0;
	while (!(c.jid == contactList[i]->jid))
		i++;
	contactList[i]->setFeatures(c.features);
}

void XmppWin::prcentChanged(Jid& jid, QString& fileName, int prc)
{
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(jid))
		{
			contactList[i]->setTranferFileState(fileName, prc); //FileName should really be an ID
			break;
		}
	}
}
