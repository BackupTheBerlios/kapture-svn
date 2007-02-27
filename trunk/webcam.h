#ifndef _WEBCAM_H
#define _WEBCAM_H

#include <QObject>
#include <QImage>
#include <QSocketNotifier>

#include <linux/videodev.h>

class Webcam : public QObject
{
	Q_OBJECT

public:
	Webcam();
	~Webcam();
	
	void close();
	int open(char *devFile);
	QList<int> getFormatList();
	int setFormat(unsigned int width, unsigned int height);
	int streamOff();
	int stopStreaming();
	int getFrame(QImage *image);
	int currentWidth();
	int currentHeight();
	int changeCtrl(int ctrl, int value);
	int defaultCtrlVal(unsigned int control);
	
	uchar *mem[2];
	size_t bufLength;
	QSocketNotifier *imageNotifier;
	QImage *image;
	bool isStreaming;
	bool isOpened;
	bool mmaped;

signals:
	void imageReady();

public slots:
	int startStreaming();

private:
	int dev;
	struct v4l2_format fmt;
	struct v4l2_buffer buf;
	struct v4l2_requestbuffers rb;
	bool allocated;
};
#endif

