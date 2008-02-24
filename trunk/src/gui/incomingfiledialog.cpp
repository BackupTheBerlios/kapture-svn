#include "incomingfiledialog.h"
#include <QDir>

IncomingFileDialog::IncomingFileDialog()
{
	ui.setupUi(this);
	ui.saveFileNameEdit->setText(QDir::homePath() + "/");
	connect(ui.agreeBtn, SIGNAL(clicked()), this, SIGNAL(agree()));
	connect(ui.declineBtn, SIGNAL(clicked()), this, SIGNAL(decline()));
}

IncomingFileDialog::~IncomingFileDialog()
{

}

void IncomingFileDialog::setFrom(const Jid& from)
{
	setWindowTitle(windowTitle() + " from " + from.full());
	f = from;
}

void IncomingFileDialog::setFileName(const QString& filename)
{
	ui.fileNameLabel->setText(filename);
	ui.saveFileNameEdit->setText(ui.saveFileNameEdit->text() + filename);
}

void IncomingFileDialog::setFileSize(int size)
{
	ui.fileSizeLabel->setText(QString("%1 octets").arg(size));
}

void IncomingFileDialog::setDesc(const QString& desc)
{
	if (desc == "")
	{
		delete ui.label_3;
		delete ui.descriptionText;
		this->resize(this->width(), 139);
	}
	else
		ui.descriptionText->insertPlainText(desc);
}

QString IncomingFileDialog::fileName() const
{
	return ui.fileNameLabel->text();
}

QString IncomingFileDialog::saveFileName() const
{
	return ui.saveFileNameEdit->text();
}

Jid IncomingFileDialog::from() const
{
	return f;
}
