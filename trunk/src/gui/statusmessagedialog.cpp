#include "statusmessagedialog.h"

StatusMessageDialog::StatusMessageDialog()
{
	ui.setupUi(this);
	connect(ui.agreeBtn, SIGNAL(clicked()), this, SIGNAL(agree()));
	connect(ui.declineBtn, SIGNAL(clicked()), this, SIGNAL(decline()));
}

StatusMessageDialog::~StatusMessageDialog()
{

}

void StatusMessageDialog::accept()
{
	emit ok();
}

QString StatusMessageDialog::statusMessage()
{
	return ui.statusMessageEdit->text();
}
