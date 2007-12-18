#include <QMessageBox>
#include <QDir>
#include <QFile>

#include "xmppwin.h"
#include "message.h"
#include "presence.h"

XmppWin::XmppWin()
{
	ui.setupUi(this);
	ui.statusBox->setCurrentIndex(Offline);
	connect(ui.statusBox, SIGNAL(currentIndexChanged(int)), this, SLOT(statusChanged()));
	connect(ui.configBtn, SIGNAL(clicked()), this, SLOT(showConfigDial()));
	ui.tableView->verticalHeader()->hide();
	ui.tableView->horizontalHeader()->hide();
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);

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
	secs = 0;
	emoticons = new Emoticons();
	connectionStatus = Offline;
	firstShow = "";
}

XmppWin::~XmppWin()
{

}

void XmppWin::statusChanged()
{
	switch (ui.statusBox->currentIndex())
	{
	case Online :
		if (connectionStatus == Offline)
		{
			jabberConnect();
		}
		else
		{
			if (connectionStatus != Xa)
			{
				connectionStatus = Online;
				setPresence();
			}
		}
		break;
	case Away :
		if (connectionStatus == Offline)
		{
			jabberConnect();
			firstShow = "away";
		}
		else
		{
			if (connectionStatus != Away)
			{
				connectionStatus = Away;
				setPresence();
			}
		}
		break;
	case Dnd :
		if (connectionStatus == Offline)
		{
			jabberConnect();
			firstShow = "dnd";
		}
		else
		{
			if (connectionStatus != Dnd)
			{
				connectionStatus = Dnd;
				setPresence();
			}
		}
		break;
	case Chat :
		if (connectionStatus == Offline)
		{
			jabberConnect();
			firstShow = "chat";
		}
		else
		{
			if (connectionStatus != Chat)
			{
				connectionStatus = Chat;
				setPresence();
			}
		}
		break;
	case Xa :
		if (connectionStatus == Offline)
		{
			jabberConnect();
			firstShow = "xa";
		}
		else
		{
			if (connectionStatus != Xa)
			{
				connectionStatus = Xa;
				setPresence();
			}
		}
		break;
	case Offline :
		if (connectionStatus != Offline)
		{
			jabberDisconnect();
		}
		break;
	}
}

void XmppWin::setPresence()
{
	QString show;
	QString status = "";
	switch(connectionStatus)
	{
	case Away:
		show = "away";
		break;
	case Chat:
		show = "chat";
		break;
	case Dnd:
		show = "dnd";
		break;
	case Xa:
		show = "xa";
		break;
	case Online:
		show = "";
		break;
	default:
		show = "";
	}
	client->setPresence(show, status);
}

/*!
 * Connects and authenticates the User to the server.
 */

void XmppWin::jabberConnect()
{
	jid = new Jid(pJid);
	
	if (!jid->isValid())
	{
		QMessageBox::critical(this, tr("Jabber"), tr("This is an invalid Jid."), QMessageBox::Ok);
		jabberDisconnect();
		printf("Invalid jid !\n");
		return;
	}
	
	client = new Client(*jid, serverEdit, portEdit);
	connect(client, SIGNAL(prcentChanged(Jid&, QString&, int)), this, SLOT(prcentChanged(Jid&, QString&, int)));
	waitingTimer = new QTimer();
	waitingTimer->start(1000);
	connect(waitingTimer, SIGNAL(timeout()), this, SLOT(connectingLogo()));
	connect(client, SIGNAL(error(Xmpp::ErrorType)), this, SLOT(error(Xmpp::ErrorType)));
	connect(client, SIGNAL(connected()), this, SLOT(clientAuthenticated()));
	client->setResource(jid->resource());
	client->setPassword(password);
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
	delete client;
	delete jid;
	Presence pr(QString(""), QString(""), QString(""));
	for (int i = 0; i < contactList.count(); i++)
	{
		contactList[i]->setPresence(pr);
	}
	m->setData(contactList);
	for (int i = 0; i < contactList.count(); i++)
	{
		ui.tableView->update(m->index(i, 0));
		ui.tableView->update(m->index(i, 1));
	}
	ui.tlsIconLabel->setEnabled(false);
	setWindowTitle("Kapture [Offline]");
	connectionStatus = Offline;
}

void XmppWin::clientAuthenticated()
{
	QPixmap *pixmap;

	if (client->isSecured())
	{
		pixmap = new QPixmap(QString(DATADIR) + QString("/icons/") + "encrypted.png");
		ui.tlsIconLabel->setToolTip(tr("The connection with the server is encrypted."));
	}
	else
	{
		pixmap = new QPixmap(QString(DATADIR) + QString("/icons/") + "decrypted.png");
		ui.tlsIconLabel->setToolTip(tr("The connection with the server is *not* encrypted."));
	}
	ui.tlsIconLabel->setPixmap(*pixmap);
	ui.tlsIconLabel->setEnabled(true);

	client->getRoster();
	connect(client, SIGNAL(rosterReady(Roster)), this, SLOT(setRoster(Roster)));
	connect(client, SIGNAL(presenceReady(const Presence&)), this, SLOT(processPresence(const Presence&)));
	connect(client, SIGNAL(messageReady(const Message&)), this, SLOT(processMessage(const Message&)));
	connectionStatus = Online;
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
		contactList[i]->setEmoticons(emoticons);
		connect(contactList[i], SIGNAL(sendMessage(QString&, QString&)), this, SLOT(sendMessage(QString&, QString&)));
		connect(contactList[i], SIGNAL(sendFileSignal(QString&)), this, SLOT(sendFile(QString&)));
		connect(contactList[i], SIGNAL(sendVideo(QString&)), this, SLOT(sendVideo(QString&)));
	}
	sortContactList();
	m->setData(contactList);
	ui.tableView->setModel(m);
	connect(ui.tableView, SIGNAL(doubleClicked(const QString&)), this, SLOT(startChat(const QString&)));
	connect(ui.tableView, SIGNAL(leftClick(const QString&, const QPoint&)), this, SLOT(showMenu(const QString&, const QPoint&)));
	if (ui.statusBox->currentIndex() != Invisible)
	{
		QString a = firstShow;
		QString b = "";
		QString c = "";
		client->setInitialPresence(a, b, c);
	}
	else
	{
		QString a = "";
		QString b = "";
		QString c = "unavailable";
		client->setInitialPresence(a, b, c);
		//FIXME:Maybe we should ask for each contact's presence.
	}
	ui.tableView->setColumnWidth(0, 22);
	ui.tableView->resizeColumnsToContents();
	waitingTimer->stop();
	delete waitingTimer;
	setWindowTitle("Kapture -- " + jid->full());
}

Contact* XmppWin::contactWithJid(const Jid& cJid)
{
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(cJid))
			return contactList[i];
	}
	return new Contact("");
}

void XmppWin::showMenu(const QString& to, const QPoint& point)
{
	menuTo = to;
	printf("[XMPPWIN] Show Menu.\n");
	QMenu *menu = new QMenu(0);
	menu->setTitle(to);
	QAction *vCardAction = menu->addAction("View vCard");
	connect(vCardAction, SIGNAL(triggered()), this, SLOT(showvCard()));
	menu->addSeparator();
	QAction *sChatAction = menu->addAction(QString("Start chat with ") + to);
	if (contactWithJid(Jid(to))->isAvailable())
		connect(sChatAction, SIGNAL(triggered()), this, SLOT(startChatFromMenu()));
	else
	{
		sChatAction->setEnabled(false);
	}
	menu->popup(point);
	menu->show();
	//FIXME:The menu should be destroid when it is clicked.
}

void XmppWin::startChatFromMenu()
{
	startChat(menuTo);
}

void XmppWin::showvCard()
{
	printf("[XMPPWIN] Show vCard of %s\n", menuTo.toLatin1().constData());
}

void XmppWin::sendVideo(QString& to)
{
	client->sendVideo(to);
}

bool sortFct(Contact *c1, Contact *c2)
{
	if (c1->isAvailable() == c2->isAvailable())
	{
		return c1 <= c2;
	}
	return c1->isAvailable() || !c2->isAvailable();
}

void XmppWin::sortContactList()
{
	qSort(contactList.begin(), contactList.end(), sortFct);
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
			//QString status = presence.status();
			//QString type = presence.type();
			contactList[i]->setPresence(presence);
			sortContactList();
			m->setData(contactList);
			for (int j = 0; j < contactList.count(); j++)
			{
				ui.tableView->update(m->index(j, 0));
				ui.tableView->update(m->index(j, 1));
			}
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
	if (connectionStatus != Offline)
		client->sendMessage(to, message);
	else
		QMessageBox::critical(this, tr("Jabber"), tr("You are not logged in right now !!!"), QMessageBox::Ok);
}

void XmppWin::startChat(const QString &sTo)
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
	ui.statusBox->setCurrentIndex(Offline);
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
	pJid = profilesa[p].jid();
	password = profilesa[p].password();
	serverEdit = profilesa[p].personnalServer();
	portEdit = profilesa[p].port();
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
		changeProfile(0);
	}
}

void XmppWin::sendFile(QString &to)
{
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

void XmppWin::closeEvent(QCloseEvent*)
{
	printf("\n * Exiting...\n");
	if (connectionStatus != Offline)
		jabberDisconnect();
	((QApplication*) this->parentWidget())->quit();
}
