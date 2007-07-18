#include "xmppconfigdialog.h"
#include <QAbstractItemModel>

XmppConfigDialog::XmppConfigDialog()
{
	ui.setupUi(this);
	Config *conf = new Config();
	profiles = conf->getProfileList();
	model = new ProfileModel();
	model->setProfileList(profiles);
	ui.profilesTable->setModel(model);
	ui.profilesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.profilesTable->setSelectionMode(QAbstractItemView::SingleSelection);
	connect(ui.profilesTable, SIGNAL(Clicked(QString)), this, SLOT(selectChange(QString)));
}

XmppConfigDialog::~XmppConfigDialog()
{

}

void XmppConfigDialog::selectChange(QString profileName)
{
	int i = 0;
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
