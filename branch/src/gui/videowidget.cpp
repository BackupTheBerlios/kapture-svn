#include <QPicture>
#include "videowidget.h"

class VideoWidgetPrivate
{
public:
	VideoWidgetPrivate()
		: f(VideoWidget::MJPEG), c(VideoWidget::Saturation)
	{};
	~VideoWidgetPrivate()
	{};

	VideoWidget::Format f;
	VideoWidget::Control c;
};

VideoWidget::VideoWidget()
{
	setText("Video Goes Here...");
	resize(320, 240);
	setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
	setMinimumSize(320, 240);
	d = new VideoWidgetPrivate();
}

VideoWidget::~VideoWidget()
{
	camera->close();
	delete camera;
	delete d;
}


bool VideoWidget::initCamera()
{
	//Looking for a device :
	camera = new Webcam();
	for (int i = 0; i < 5; i++)
	{
		if (EXIT_SUCCESS == camera->open(QString("/dev/video%1").arg(i)))
			break;
	}
	if (!camera->isOpened())
	{
		for (int i = 0; i < 5; i++)
		{
			if (EXIT_SUCCESS == camera->open(QString("/dev/v4l/video%1").arg(i)))
				break;
		}
	}
	if (!camera->isOpened())
		return false;
	/*if (!setFormat())
		return false;
	if (!setSize())
		return false;*/
	
	return true;
}

bool VideoWidget::setFormat(VideoWidget::Format)
{

}

bool VideoWidget::setSize(const QSize&)
{

}

void VideoWidget::play()
{
	QPixmap *pix = new QPixmap("/home/detlev/images/Charles Gosselin/test.jpg");
	*pix = pix->scaledToHeight(240);
	*pix = pix->scaledToWidth(320);
	setPixmap(*pix);
}

void VideoWidget::stop()
{

}

void VideoWidget::setControl(VideoWidget::Control, int)
{

}

VideoWidget::Format VideoWidget::format() const
{
	return d->f;
}

VideoWidget::Control VideoWidget::control() const
{
	return d->c;
}

void VideoWidget::showImage()
{

}
