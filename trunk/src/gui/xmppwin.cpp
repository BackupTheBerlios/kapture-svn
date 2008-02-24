#include <QMessageBox>
#include <QDir>
#include <QFile>

#include "xmppwin.h"
#include "message.h"
#include "presence.h"
#include "joystick.h"

XmppWin::XmppWin()
{
	ui.setupUi(this);
	QIcon i(QString(DATADIR) + QString("/icons/16x16.png"));
	setWindowIcon(i);
	ui.statusBox->setCurrentIndex(Offline);
	connect(ui.statusBox, SIGNAL(currentIndexChanged(int)), this, SLOT(statusChanged()));
	connect(ui.configBtn, SIGNAL(clicked()), this, SLOT(showConfigDial()));
	ui.tableView->verticalHeader()->hide();
	ui.tableView->horizontalHeader()->hide();
	ui.tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableView->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.addItemBtn->setEnabled(false);

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
	updateConfig();
	
	connect(ui.profilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeProfile(int)));
	secs = 0;
	emoticons = new Emoticons();
	connectionStatus = Offline;
	firstShow = "";

	useSystemTray = conf->useSystemTray();
	res = conf->resource();
	if (useSystemTray)
	{
		/* Concerns QSystemTrayIcon */
		sti = new QSystemTrayIcon();
		sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/offline.png")));
		sti->setToolTip(QString("Kapture"));

		sysTrayMenu = new QMenu(0);
		sysTrayMenu->setTitle(QString("Kapture -- ") + pJid.full());
		for (int i = 0; i < conf->profileList().count(); i++)
		{
			QAction *contactActionMenu = sysTrayMenu->addAction(conf->profileList().at(i).name());
			connect(contactActionMenu, SIGNAL(triggered()), this, SLOT(jabberConnect()));
		}
		sysTrayMenu->addSeparator();
		QAction *sQuitAction = sysTrayMenu->addAction(QString("Quit"));
		connect(sQuitAction, SIGNAL(triggered()), this, SLOT(quitApp()));
		sysTrayMenu->setTitle("Kapture");
		sti->setContextMenu(sysTrayMenu);
		connect(sti, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(systrayActivated(QSystemTrayIcon::ActivationReason)));
		sti->show();
	}
	
	waitingTimer = new QTimer();

	// Concerns Joystick.
	//Joystick *js0 = new Joystick("/dev/js0");
}

XmppWin::~XmppWin()
{

}

void XmppWin::systrayActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::Trigger)
	{
		if (!isVisible())
			show();
		else
			hide();
	}
}

void XmppWin::quitApp()
{
	printf("\n * Exiting...\n");
	if (connectionStatus != Offline)
		jabberDisconnect();
	((QApplication*) this->parentWidget())->quit();
}

void XmppWin::statusChanged()
{
	/*
	 * There should be a way to cancel a connection that, for example, went wrong.
	 */
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
		sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/away.png")));
		break;
	case Chat:
		show = "chat";
		sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/chat.png")));
		break;
	case Dnd:
		show = "dnd";
		sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/dnd.png")));
		break;
	case Xa:
		show = "xa";
		sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/xa.png")));
		break;
	case Online:
		show = "";
		sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/online.png")));
		break;
	default:
		show = "";
		sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/online.png")));
	}
	client->setPresence(show, status);
}

/*!
 * Connects and authenticates the User to the server.
 */

void XmppWin::jabberConnect()
{
	jid = new Jid(pJid.bare() + QString("/") + res);
	
	if (!jid->isValid())
	{
		QMessageBox::critical(this, tr("Jabber"), tr("This is an invalid Jid."), QMessageBox::Ok);
		jabberDisconnect();
		printf("Invalid jid !\n");
		return;
	}
	
	client = new Client(*jid, serverEdit, portEdit);
	connect(client, SIGNAL(prcentChanged(Jid&, QString&, int)), this, SLOT(prcentChanged(Jid&, QString&, int)));
	connect(waitingTimer, SIGNAL(timeout()), this, SLOT(connectingLogo()));
	waitingTimer->start(1000);
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
	m->setData(&contactList);
	for (int i = 0; i < contactList.count(); i++)
	{
		ui.tableView->update(m->index(i, 0));
		ui.tableView->update(m->index(i, 1));
	}
	ui.tlsIconLabel->setEnabled(false);
	setWindowTitle("Kapture [Offline]");
	connectionStatus = Offline;
	sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/offline.png")));
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
	connect(client, SIGNAL(signalUpdateItem(Contact*)), SLOT(updateRosterItem(Contact*)));
	connectionStatus = Online;
	sti->setIcon(QIcon(QString(DATADIR) + QString("/icons/online.png")));
	sti->showMessage("Kapture - Connected", QString("You are now connected with %1.").arg(pJid.full()));
}

void XmppWin::updateRosterItem(Contact *c)
{
	printf("[XMPPWIN] updateRosterItem();\n");
	Contact *contact = contactWithJid(*(c->jid));
	if (contact == NULL)
	{
		printf("[XMPPWIN] Add %s\n", c->jid->full().toLatin1().constData());
		c->setEmoticons(emoticons);
		connect(c, SIGNAL(sendMessage(QString&, QString&)), this, SLOT(sendMessage(QString&, QString&)));
		connect(c, SIGNAL(sendFileSignal(QString&)), this, SLOT(sendFile(QString&)));
		connect(c, SIGNAL(sendVideo(QString&)), this, SLOT(sendVideo(QString&)));

		contactList << c;
		sortContactList();
		m->setData(&contactList);
		ui.tableView->setModel(m);
		for (int j = 0; j < contactList.count(); j++)
		{
			ui.tableView->update(m->index(j, 0));
			ui.tableView->update(m->index(j, 1));
		}
		ui.tableView->resizeColumnsToContents();
	}
	else
	{
		printf("[XMPPWIN] Update %s\n", c->jid->full().toLatin1().constData());
		contact->setSubscription(c->subscription());
	}
}

#include <math.h>
#define PI (3.141592653589793)

void XmppWin::setRoster(Roster roster)
{
	/*
	 * FIXME:
	 * setRoster should behave like updateRosterItem() as when disconnecting and reconnecting,
	 * the whole contactList is rebuild and actives contacts windows become obsolete.
	 * So, if a contact already exists when receiving the roster, just update it.
	 * Otherwise, add it to the contact list. If we are reconnecting (find a way to know it, for example, contact list not empty), contacts
	 * present in the list but not present in the new received roster should be removed (and their window eventually cloased)
	 */
	r = roster;
	m = new Model();
	d = new ContactWidgetDelegate(); 
	contactList.clear();
	contactList = r.contactList();
	// Connecting contacts.
	gScene = new QGraphicsScene(ui.graphicsView);
	for (int i = 0; i < contactList.count(); i++)
	{
		contactList[i]->setEmoticons(emoticons);
		connect(contactList[i], SIGNAL(sendMessage(QString&, QString&)), this, SLOT(sendMessage(QString&, QString&)));
		connect(contactList[i], SIGNAL(sendFileSignal(QString&)), this, SLOT(sendFile(QString&)));
		connect(contactList[i], SIGNAL(sendVideo(QString&)), this, SLOT(sendVideo(QString&)));

		QGraphicsTextItem *text = gScene->addText(contactList[i]->jid->full());
		text->setPos(30*sin(i*PI/4), i*15);

		QPolygonF polygonF;
		polygonF << QPointF(-5, -5) << QPointF(5, -5) << QPointF(0, +5);
		gScene->addPolygon(polygonF)->setPos(150, (i*15)+7.5);
	}
	
	
	connect(ui.tableView, SIGNAL(doubleClicked(const Jid&)), this, SLOT(startChat(const Jid&)));
	connect(ui.tableView, SIGNAL(rightClick(const Jid&, const QPoint&)), this, SLOT(showMenu(const Jid&, const QPoint&)));
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
	}
	waitingTimer->stop();
	setWindowTitle("Kapture -- " + jid->full());
	ui.addItemBtn->setEnabled(true);
	connect(ui.addItemBtn, SIGNAL(clicked()), SLOT(addItem()));

	ui.graphicsView->setScene(gScene);

	sortContactList();
	m->setData(&contactList);
	ui.tableView->setModel(m);
	ui.tableView->setItemDelegate(d);
	
	ui.tableView->setColumnWidth(0, 22);
	ui.tableView->resizeColumnsToContents();
}

void XmppWin::addItem()
{
	bool ok;
	QString text = QInputDialog::getText(this, tr("Adding a contact"), tr("Jid [e.g. cazou88@jabber.org] :"), QLineEdit::Normal, QString(), &ok);
	if (ok && Jid(text).isValid())
	{
		client->addItem(Jid(text), QString(""), QString("")); //FIXME:Should create a dialog to get the name and the group.
	}
}

Contact* XmppWin::contactWithJid(const Jid& cJid)
{
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(cJid))
			return contactList[i];
	}
	return NULL;
}

void XmppWin::showMenu(const Jid& to, const QPoint& point)
{
	menuTo = to;
	printf("[XMPPWIN] Show Menu.\n");
	menu = new QMenu(0);
	menu->setTitle(to.full());
	QAction *vCardAction = menu->addAction("View vCard");
	connect(vCardAction, SIGNAL(triggered()), this, SLOT(showvCard()));
	menu->addSeparator();
	QAction *sChatAction = menu->addAction(QString("Start chat with ") + to.full());
	if (contactWithJid(to)->isAvailable())
		connect(sChatAction, SIGNAL(triggered()), this, SLOT(startChatFromMenu()));
	else
		sChatAction->setEnabled(false);

	QMenu *mSubsMenu = new QMenu(0);
	mSubsMenu->setTitle("Manage Subscriptions");
	menu->addMenu(mSubsMenu);
	QAction *delAction = menu->addAction("Delete contact");
	connect(delAction, SIGNAL(triggered()), SLOT(delItem()));

	if (contactWithJid(to)->subscription() == "both")
	{
		QAction *addAuthAction = mSubsMenu->addAction(QString("Resend authorization to ") + to.full());
		connect(addAuthAction, SIGNAL(triggered()), this, SLOT(slotAddAuth()));
		QAction *remAuthAction = mSubsMenu->addAction(QString("Remove authorization of ") + to.full());
		connect(remAuthAction, SIGNAL(triggered()), this, SLOT(slotRemAuth()));
	}

	if (contactWithJid(to)->subscription() == "to")
	{
		QAction *addAuthAction = mSubsMenu->addAction(QString("Resend authorization to ") + to.full());
		connect(addAuthAction, SIGNAL(triggered()), this, SLOT(slotAddAuth()));
	}

	if (contactWithJid(to)->subscription() == "from")
	{
		QAction *reqAuthAction = mSubsMenu->addAction(QString("Request authorization from ") + to.full());
		connect(reqAuthAction, SIGNAL(triggered()), this, SLOT(slotReqAuth()));
		QAction *remAuthAction = mSubsMenu->addAction(QString("Remove authorization of ") + to.full());
		connect(remAuthAction, SIGNAL(triggered()), this, SLOT(slotRemAuth()));
	}
	
	if (contactWithJid(to)->subscription() == "none")
	{
		QAction *addAuthAction = mSubsMenu->addAction(QString("Resend authorization to ") + to.full());
		connect(addAuthAction, SIGNAL(triggered()), this, SLOT(slotAddAuth()));
		QAction *reqAuthAction = mSubsMenu->addAction(QString("Request authorization from ") + to.full());
		connect(reqAuthAction, SIGNAL(triggered()), this, SLOT(slotReqAuth()));
	}
	menu->popup(point);
	menu->show();
	//FIXME:The menu should be destroid when it is clicked.
}

void XmppWin::delItem()
{
	client->delItem(menuTo);
}

void XmppWin::slotAddAuth()
{
	client->addAuthFor(menuTo.full());
	delete menu;
}

void XmppWin::slotRemAuth()
{
	client->removeAuthFor(menuTo.full());
	delete menu;
}

void XmppWin::slotReqAuth()
{
	client->requestAuthFor(menuTo.full());
	delete menu;
}

void XmppWin::startChatFromMenu()
{
	startChat(menuTo.full());
	delete menu;
}

void XmppWin::showvCard()
{
	printf("[XMPPWIN] Show vCard of %s\n", menuTo.full().toLatin1().constData());
}

void XmppWin::sendVideo(QString& to)
{
	client->sendVideo(to);
}

bool sortFct(Contact *c1, Contact *c2)
{
	/*if (c1->show() == c2->show())
	{
		return c1->jid->bare() > c2->jid->bare();
	}

	if (c1->isAvailable() && c2->isAvailable())
	{
		return !(c1->show() != "" && c2->show() == "");
	}

	if (!c1->isAvailable() || !c2->isAvailable())
	{
		return c1->jid->bare() > c2->jid->bare();
	}*/

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
			// Check if the presence has changed.
			if (contactList[i]->presence()->type() != presence.type())
			{
				sti->showMessage("Kapture", 
					QString("%1 is now %2").arg(presence.from().full()).arg(presence.type() == "unavailable" ? "Offline" : "Online"));
			}

			// Set eventual new resource for this contact.
			// FIXME: should be done only if the contact has no resource yet.
			// TODO: should add the resource to a resourceList for this contact as it can be connected with more than 1 resource.
			contactList[i]->jid->setResource(presence.from().resource());

			// Set contact's presence.
			contactList[i]->setPresence(presence);

			// Reset contactList order.
			sortContactList();

			// Updating Table View.
			//m->setData(contactList);
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

void XmppWin::startChat(const Jid& to)
{
	for (int i = 0; i < contactList.count(); i++)
	{
		if (contactList[i]->jid->equals(to))
		{
			contactList[i]->startChat();
		}
	}
}

void XmppWin::error(Xmpp::ErrorType e)
{
	// Still a lot of errors to manage...
	// Errors from the authentification process.
	if (waitingTimer->isActive())
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
}

void XmppWin::showConfigDial()
{
	XmppConfigDialog *dial = new XmppConfigDialog();
	dial->show();
	connect(dial, SIGNAL(accepted()), this, SLOT(updateConfig()));
}

void XmppWin::changeProfile(int p)
{
	pJid = profileList[p].jid();
	password = profileList[p].password();
	serverEdit = profileList[p].personnalServer();
	portEdit = profileList[p].port();
}

void XmppWin::updateConfig()
{
	// Update Profiles
	// Loads predifined Profiles.
	delete conf;
	disconnect(ui.profilesComboBox, SIGNAL(currentIndexChanged(int)), 0, 0);
	ui.profilesComboBox->clear();
	connect(ui.profilesComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeProfile(int)));
	conf = new Config();
	if (conf->noConfig)
		return;
	
	profileList = conf->profileList();
	
	for (int i = 0; i < profileList.count(); i++)
	{
		ui.profilesComboBox->addItem(profileList[i].name());
	}
	
	if (profileList.count() > 0)
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

void XmppWin::closeEvent(QCloseEvent* event)
{
	if (!useSystemTray)
	{
		printf("\n * Exiting...\n");
		if (connectionStatus != Offline)
			jabberDisconnect();
		((QApplication*) this->parentWidget())->quit();
	}
	else
	{
		event->ignore();
		hide();
	}
}
