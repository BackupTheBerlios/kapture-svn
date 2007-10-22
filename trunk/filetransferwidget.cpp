#include "filetransferwidget.h"

FileTransferWidget::FileTransferWidget()
{

}

FileTransferWidget::FileTransferWidget(QString& fileName)
{
	f = fileName;

	fileTransferBar = new QProgressBar();
	fileTransferBar->setAlignment(Qt::AlignHorizontal_Mask);
	fileTransferBar->setRange(0, 100);

	lab = new QLabel();
	lab->setScaledContents(true);
	fileName.truncate(20);
	lab->setText(fileName + "..." + " : ");
	
	hboxlayout = new QHBoxLayout();
	hboxlayout->addWidget(lab);
	hboxlayout->addWidget(fileTransferBar);
}

FileTransferWidget::~FileTransferWidget()
{

}

void FileTransferWidget::setPourcentage(int prc)
{
	fileTransferBar->setValue(prc);
}

QHBoxLayout *FileTransferWidget::box()
{
	return hboxlayout;
}

QString FileTransferWidget::fileName() const
{
	return f;
}
