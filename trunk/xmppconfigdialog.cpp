#include <QAbstractItemModel>

#include "xmppconfigdialog.h"
#include "kapturewin.h"

XmppConfigDialog::XmppConfigDialog()
{
	ui.setupUi(this);
	conf = new Config();
	profiles = conf->profileList();
	model = new ProfileModel();
	model->setProfileList(profiles);
	ui.profilesTable->setModel(model);
	ui.profilesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.profilesTable->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.profilesTable->resizeColumnsToContents();
	connect(ui.profilesTable, SIGNAL(Clicked(QString&)), this, SLOT(selectChange(QString&)));
	connect(ui.addBtn, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui.delBtn, SIGNAL(clicked()), this, SLOT(del()));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SLOT(saveConfig()));
	connect(ui.webcamBtn, SIGNAL(clicked()), this, SLOT(configWebcam()));

	ui.ftPortEdit->setValue(conf->ftPort());
	ui.proxyEdit->setText(conf->proxy());
	ui.resourceEdit->setText(conf->resource());
	ui.useSystemTrayCheckBox->setChecked(conf->useSystemTray());
}

XmppConfigDialog::~XmppConfigDialog()
{

}

void XmppConfigDialog::saveConfig()
{
	/*save config (ftPort, proxy, resource)*/
	emit accepted();
}

void XmppConfigDialog::configWebcam()
{
	KaptureWin *kw = new KaptureWin();
	kw->show();
}

void XmppConfigDialog::selectChange(QString& profileName)
{
	int i = 0;
	selectedProfile = profileName;
	for (i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].name() == profileName)
			break;
	}
	ui.jidEdit->setText(profiles[i].jid());
	ui.passwordEdit->setText(profiles[i].password());
	ui.personnalServerEdit->setText(profiles[i].personnalServer());
	ui.portEdit->setText(profiles[i].port());
	ui.profileNameEdit->setText(profiles[i].name());
}
/*!
 * Adds the profile in the config file.
 */
void XmppConfigDialog::add()
{
	//Checking if we can add this account.
	if (ui.profileNameEdit->text() == "" || ui.jidEdit->text() == "" || ui.passwordEdit->text() == "")
	{
		QMessageBox::critical(this, tr("Profiles"), QString("You *must* supply a profile name, a JID and a password."), QMessageBox::Ok);
		return;
	}
	
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].name() == ui.profileNameEdit->text())
		{
			QMessageBox::critical(this, tr("Profiles"), QString("A profile with the name \"%1\" already exists.").arg(ui.profileNameEdit->text()), QMessageBox::Ok);
			return;
		}
	}

	QString pName = ui.profileNameEdit->text();
	QString pJid = ui.jidEdit->text();
	QString pPass = ui.passwordEdit->text();
	QString pServer = ui.personnalServerEdit->text();
	QString pPort = ui.portEdit->text();

	Profile p(pName);
	p.setData(pJid, pPass, pServer, pPort);

	if (ui.registerBox->isChecked())
	{
		// Register new account.
		xmpp = new Xmpp();
		task = new Task();
		xmppReg = new XmppReg(task, xmpp);
		connect(xmpp, SIGNAL(readyRead()), SLOT(read()));
		connect(xmppReg, SIGNAL(finished()), SLOT(regFinished()));
		connect(xmppReg, SIGNAL(error()), SLOT(registerError()));
		xmppReg->registerAccount(p);
	}
	else
	{
		// FIXME:This should be in another Method (also run after registering)
		addProfile(p);
	}
}

void XmppConfigDialog::read()
{
	while (xmpp->stanzaAvailable())
	{
		printf("Read next Stanza\n");
		Stanza *s = xmpp->getFirstStanza();
		printf("Client:: : %s\n", s->from().full().toLatin1().constData());
		task->processStanza(*s);
	}
}

void XmppConfigDialog::regFinished()
{
	addProfile(xmppReg->profile());
	QMessageBox::information(this, "Registration", "Successfully registered to the server.");
}

void XmppConfigDialog::registerError()
{
	/*
	QMessage::critical("Kapture", [...], xmppReg->errorMessage());
	*/
}

void XmppConfigDialog::addProfile(const Profile& p)
{
		conf->addProfile(p);
		delete conf;
		conf = new Config();

		profiles = conf->profileList();
		model->setProfileList(profiles);
		model->insertRow(profiles.count(), QModelIndex());
}

void XmppConfigDialog::del()
{
	int pos = ui.profilesTable->currentIndex().row();

	model->removeRow(pos, QModelIndex());
	conf->delProfile(selectedProfile);
	delete conf;
	conf = new Config();
	profiles = conf->profileList();
	model->setProfileList(profiles);
	
	// Selection has changed, swithing to the new selected profile
	pos = ui.profilesTable->currentIndex().row();
	QString profileName = model->index(pos, 0).data().toString();
	selectChange(profileName);
}
