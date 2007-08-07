#include "xmppconfigdialog.h"
#include <QAbstractItemModel>

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
	connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
}

XmppConfigDialog::~XmppConfigDialog()
{

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
	QString pName = ui.profileNameEdit->text();
	QString pJid = ui.jidEdit->text();
	QString pPass = ui.passwordEdit->text();
	QString pServer = ui.personnalServerEdit->text();
	QString pPort = ui.portEdit->text();

	Profile p(pName);
	p.setData(pJid, pPass, pServer, pPort);
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
