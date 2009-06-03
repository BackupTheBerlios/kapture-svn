#ifndef WEBCAM_CONFIG_WIDGET_H
#define WEBCAM_CONFIG_WIDGET_H

#include "ui_webcamconfigwidget.h"

class WebcamPool;
class Webcam;

class WebcamConfigWidget : public QWidget
{
	Q_OBJECT
public:
	WebcamConfigWidget(QWidget *parent = 0);
	~WebcamConfigWidget();

private:
	void startCamera(Webcam *w);
	Ui::webcamConfigWidget ui;
	WebcamPool *pool;
	Webcam *camera;
};

#endif
