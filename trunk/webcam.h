/*
 *      webcam.h -- Kapture
 *
 *      Copyright (C) 2006-2007
 *          Detlev Casanova (detlev.casanova@gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

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
	int  open(char *devFile);
	QList<int>   getFormatList(QList<QString> *description);
	QList<QSize> getSizesList();
	int setFormat(unsigned int width, unsigned int height, int pixelformat=V4L2_PIX_FMT_MJPEG);
	int streamOff();
	int stopStreaming();
	int getFrame(QImage *image);
	int currentWidth();
	int currentHeight();
	int currentPixelFormat();
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

