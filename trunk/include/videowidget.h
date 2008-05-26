#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H
#include <QWidget>
#include <QLabel>
#include <QSize>

#include "webcam.h"

class VideoWidgetPrivate;

class VideoWidget : public QLabel
{
	Q_OBJECT;
public:
	VideoWidget();
	~VideoWidget();
	
	enum Format
	{
		MJPEG = 0,
		YUYV
	};

	enum Control
	{
		Saturation = 0,
		PowerLineFreq,
		Brightness,
		Contrast,
		Sharpness,
		PanTiltReset
	};

	/*
	 * Inits the camera so it's ready to stream.
	 */
	bool initCamera();
	
	/*
	 * Sets frame format (MJPEG, YUYV,...).
	 */
	bool setFormat(VideoWidget::Format);
	
	/*
	 * Sets the frame size (320x240,...).
	 */
	bool setSize(const QSize&);
	
	/*
	 * Plays the streaming.
	 */
	void play();
	
	/*
	 * Stops the streaming
	 */
	void stop();

	/*
	 * Sets a control at a given position or increments it of 1 by default.
	 */
	//FIXME: Some devices have negative values for their controls.
	void setControl(VideoWidget::Control, int value = -1);
	
	VideoWidget::Format format() const;
	VideoWidget::Control control() const;

private:
	class VideoWidgetPrivate *d;
	Webcam *camera;

signals:
	void imageReady();

public slots:
	void showImage();
};

#endif //VIDEOWIDGET_H
