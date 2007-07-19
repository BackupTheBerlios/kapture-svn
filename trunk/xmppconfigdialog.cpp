#include "xmppconfigdialog.h"
#include <QAbstractItemModel>

XmppConfigDialog::XmppConfigDialog()
{
	ui.setupUi(this);
	conf = new Config();
	profiles = conf->getProfileList();
	model = new ProfileModel();
	model->setProfileList(profiles);
	ui.profilesTable->setModel(model);
	ui.profilesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.profilesTable->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.profilesTable->resizeColumnsToContents();
	//ui.profilesTable->setColumnWidth(0, ui.profilesTable->width() - ui.profilesTable->columnWidth(1) - ui.profilesTable->columnWidth(2) - ui.profilesTable->columnWidth(3));
	connect(ui.profilesTable, SIGNAL(Clicked(QString)), this, SLOT(selectChange(QString)));
	connect(ui.addBtn, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui.delBtn, SIGNAL(clicked()), this, SLOT(del()));
	connect(ui.buttonBox, SIGNAL(accepted()), this, SIGNAL(accepted()));
}

XmppConfigDialog::~XmppConfigDialog()
{

}

void XmppConfigDialog::selectChange(QString profileName)
{
	int i = 0;
	selectedProfile = profileName;
	for (i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profileName)
			break;
	}
	ui.jidEdit->setText(profiles[i].getJid());
	ui.passwordEdit->setText(profiles[i].getPassword());
	ui.personnalServerEdit->setText(profiles[i].getPersonnalServer());
	ui.portEdit->setText(profiles[i].getPort());
	ui.profileNameEdit->setText(profiles[i].getName());
}

void XmppConfigDialog::add()
{
	Profile p(ui.profileNameEdit->text());
	p.setData(ui.jidEdit->text(),
		  ui.passwordEdit->text(),
		  ui.personnalServerEdit->text(),
		  ui.portEdit->text());
	if (ui.profileNameEdit->text() == "" || ui.jidEdit->text() == "" || ui.passwordEdit->text() == "")
	{
		QMessageBox::critical(this, tr("Profiles"), QString("You *must* supply a profile name, a JID and a password."), QMessageBox::Ok);
		return;
	}
	
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == ui.profileNameEdit->text())
		{
			QMessageBox::critical(this, tr("Profiles"), QString("A profile with the name \"%1\" already exists.").arg(ui.profileNameEdit->text()), QMessageBox::Ok);
			return;
		}
	}

	conf->addProfile(p);
	delete conf;
	conf = new Config();

	profiles = conf->getProfileList();
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
	profiles = conf->getProfileList();
	model->setProfileList(profiles);
	
	// Selection has changed, swithing to the new selected profile
	pos = ui.profilesTable->currentIndex().row();
	QString profileName = model->index(pos, 0).data().toString();
	selectChange(profileName);
}
