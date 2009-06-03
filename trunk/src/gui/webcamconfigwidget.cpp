#include "webcamconfigwidget.h"
#include "webcampool.h"
#include "webcam.h"

WebcamConfigWidget::WebcamConfigWidget(QWidget *parent)
 : QWidget(parent)
{
	ui.setupUi(parent);
	ui.pictureLabel->setAlignment(Qt::AlignCenter);
	ui.pictureLabel->setText("Initializing the webcam...");
	ui.pictureLabel->resize(320, 280);

	pool = WebcamPool::self();
	if (pool->count() <= 0)
	{
		ui.pictureLabel->setText("No camera found...");
		return;
	}
	
	pool->fillDeviceComboBox(ui.comboBoxDevs);
	startCamera(pool->device(0));

}

WebcamConfigWidget::~WebcamConfigWidget()
{
}

void WebcamConfigWidget::startCamera(Webcam *w)
{
/*
QList<int>   getFormatList(QList<QString> &description) const;
QList<QSize> getSizesList() const;
*/
	camera = w;
	camera->open();
	QList<QString> formatName;
	QList<int> formatList = camera->getFormatList(formatName);
	ui.comboBoxFormat->clear();
	for (int i = 0; i < formatList.size(); i++)
		ui.comboBoxFormat->addItem(formatName.at(i));
	

}
