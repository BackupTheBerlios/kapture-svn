/*
 *      webcam.cpp -- Kapture
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

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>

#include <linux/videodev.h>
#include <uvcvideo.h>

#include <QtGui>
#include <QApplication>
#include <QMainWindow>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QSize>
#include <QSocketNotifier>

#include "webcam.h"
#include "imageConvert.h"
#include "merror.h"


Webcam::Webcam()
{
	isStreaming = false;
	dev = 0;
	isOpened = false;
	imageNotifier = 0;
	allocated = false;
}

Webcam::~Webcam()
{
	close();
}

void Webcam::close()
{
	stopStreaming();
	::close(dev);

	delete imageNotifier;
	imageNotifier = 0;
	isOpened = false;
	allocated = false;
}

int Webcam::open(char *devFile)
{
	struct v4l2_capability cap;
	int ret;
	char str[256];

	if (isOpened)
		close();

	dev = ::open(devFile, O_RDWR);
	if (dev < 0) 
	{
		strcpy(str, "Error Opening ");
		KError(strcat(str, devFile), errno);
		return EXIT_FAILURE;
	}

        memset(&cap, 0, sizeof cap);
        ret = ioctl(dev, VIDIOC_QUERYCAP, &cap);
        if (ret < 0) 
	{
		strcpy(str, "Error querying capabilities for ");
		KError(strcat(str, devFile), errno);
                return EXIT_FAILURE;
	}

	if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) == 0) 
	{
		strcpy(str, "Error checking caps for ");
		KError(strcat(str, devFile), errno);
                return -EINVAL;
	}
	isOpened = true;

	imageNotifier = new QSocketNotifier(dev, QSocketNotifier::Read);
	imageNotifier->setEnabled(false);
	connect(imageNotifier, SIGNAL(activated(int)), this, SIGNAL(imageReady()));
	return EXIT_SUCCESS;
}

QList<int> Webcam::getFormatList(QList<QString> *description)
{
	QList<int> formatList;
	int ret;
	struct v4l2_fmtdesc fmtList;
	memset(&fmtList, 0, sizeof fmtList);
	fmtList.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int i = 0;

	do
	{
		fmtList.index = i;
		if ((ret = ioctl(dev, VIDIOC_ENUM_FMT, &fmtList)) < 0)
			break;
		else
		{
			formatList.append((int)fmtList.pixelformat);
			description->append((char*)fmtList.description);
		}
		i++;
	}
	while (ret != EINVAL);
	return formatList;
}

QList<QSize> Webcam::getSizesList()
{
	int i = 0;
	QList<QSize> rSizes;
	QSize tmp;
	struct v4l2_frmsizeenum sizes;
	memset(&sizes, 0, sizeof sizes);
	sizes.pixel_format = currentPixelFormat();
	sizes.index = i;
	while(ioctl(dev, VIDIOC_ENUM_FRAMESIZES, &sizes) != -1)
	{
		tmp.setWidth((int)sizes.discrete.width);
		tmp.setHeight((int)sizes.discrete.height);
		rSizes.append(tmp);
		i++;
		sizes.index = i;
	}
	return rSizes;

}

int Webcam::setFormat(unsigned int width, unsigned int height, int pixelformat)
{
	int ret;
	int i = 0;

	if(isStreaming)
		return -1;

	memset(&fmt, 0, sizeof fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = width;
	fmt.fmt.pix.height = height;
	fmt.fmt.pix.field = V4L2_FIELD_ANY;
	fmt.fmt.pix.pixelformat = pixelformat;
	if (ioctl(dev, VIDIOC_S_FMT, &fmt) < 0)
	{
		KError("Error while setting format", errno);
		i++;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

int Webcam::streamOff()
{
	int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	int ret;
	
	if(!isStreaming)
		return -1;
	
	ret = ioctl(dev, VIDIOC_STREAMOFF, &type);
	if (ret < 0) 
	{
		KError("Unable to stop capture", errno);
		return EXIT_FAILURE;
	}
	isStreaming = false;
	return EXIT_SUCCESS;
}

int Webcam::startStreaming()
{
	int i, ret;

	if (!isOpened)
		return -1;

	if ((ret = setFormat(currentWidth(), currentHeight(), currentPixelFormat())) != 0)
	{
		printf("set format error : %d\n", ret);
		return EXIT_FAILURE;
	}

	//Allocate buffers 
	if (!allocated)
	{
		memset(&rb, 0, sizeof rb);
		rb.count = 2;
		rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		rb.memory = V4L2_MEMORY_MMAP;
	
		ret = ioctl(dev, VIDIOC_REQBUFS, &rb);
		if (ret < 0) 
		{
			KError("Unable to allocate buffers", errno);
			return EXIT_FAILURE;
		}
		allocated = true;
	}

			
	// Map the buffers. /
	memset(&buf, 0, sizeof buf);
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	for (i = 0; i < 2; i++)
	{
		buf.index = i; 
		ret = ioctl(dev, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			KError("Unable to query buffer", errno);
			return EXIT_FAILURE;
		}
		
		mem[i] = (uchar *) mmap(0, buf.length, PROT_READ, MAP_SHARED, dev, buf.m.offset);
		if (mem[i] == MAP_FAILED) {
			KError("Unable to map buffer", errno);
			return EXIT_FAILURE;
		}
		bufLength = buf.length;
		mmaped = true;
	}

	// Queue the buffers. /
	for (i = 0; i < 2; i++)
	{
		buf.index = i;
		ret = ioctl(dev, VIDIOC_QBUF, &buf);
		if (ret < 0) 
		{
			KError("Unable to queue buffer", errno);
			return EXIT_FAILURE;
		}
	}

	// Start streaming.
	ret = ioctl(dev, VIDIOC_STREAMON, &buf.type);
	if (ret < 0) 
	{
		KError("Unable to start capture", errno);
		return EXIT_FAILURE;
	}
	
	imageNotifier->setEnabled(true);
	isStreaming = true;
	return EXIT_SUCCESS;
}

int Webcam::stopStreaming()
{
	if(!isStreaming)
		return -1;
	
	imageNotifier->setEnabled(false);
	
	if (munmap(mem[0], bufLength) == -1)
	{
		printf("Webcam::stopStreaming : munmap 0 failed. errno = %d\n", errno);
	}

	if (munmap(mem[1], bufLength) == -1)
	{
		printf("Webcam::stopStreaming : munmap 1 failed. errno = %d\n", errno);
	}
	else
		mmaped = false;

	if(streamOff() == 0)
	{
		isStreaming = false;
		printf(" * Succesful Stopped\n");
	}
	else
		imageNotifier->setEnabled(true);
	return EXIT_SUCCESS;
}

int Webcam::getFrame(QImage *image)
{
	int ret = 0;

	// Dequeue a buffer.
	ret = ioctl(dev, VIDIOC_DQBUF, &buf);
	if (ret < 0) 
	{
		KError("Unable to dequeue buffer", errno);
		return EXIT_FAILURE;
	}

	// Save the image.
	uchar jpegBuf1[buf.bytesused + 420];
	if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_MJPEG)
	{
		if (mjpegToJpeg(mem[buf.index], jpegBuf1, (int) buf.bytesused) == EXIT_SUCCESS)
			image->loadFromData(jpegBuf1, buf.bytesused+420);
	}

	if (fmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV)
	{
		yuvToJpeg(mem[buf.index], image, currentWidth(), currentHeight());
	}
	
	// Requeue the buffer.
	ret = ioctl(dev, VIDIOC_QBUF, &buf);
	if (ret < 0) 
	{
		KError("Unable to requeue buffer", errno);
		return EXIT_FAILURE;
	}
	if(!imageNotifier->isEnabled())
		imageNotifier->setEnabled(true);
	
	return EXIT_SUCCESS;
}

int Webcam::changeCtrl(int ctrl, int value)
{
	struct v4l2_queryctrl queryctrl;
	struct v4l2_control control;
	
	if(!isOpened) // At the begining of the function.
	{
		return -1;
	}
/*
 * ctrl values :
 * 	0 : Saturation
 * 	1 : Power line Frequency (néons)
 * 	2 : Brightness
 * 	3 : Contrast
 * 	4 : Sharpness
 */
	__u32 CTRL;
	switch(ctrl)
	{
		case 0: 
		{
			CTRL = V4L2_CID_SATURATION;
			break;
		}
		case 1: 
		{
			CTRL = V4L2_CID_POWER_LINE_FREQUENCY;
			break;
		}
		case 2: 
		{
			CTRL = V4L2_CID_BRIGHTNESS;
			break;
		}
		case 3: 
		{
			CTRL = V4L2_CID_CONTRAST;
			break;
		}
		case 4: 
		{
			CTRL = V4L2_CID_SHARPNESS;
			break;
		}
		default:
		{
			break;
		}
	}

	memset (&queryctrl, 0, sizeof queryctrl);
	queryctrl.id = CTRL;
	if (-1 == ioctl (dev, VIDIOC_QUERYCTRL, &queryctrl)) 
	{
	        if (errno != EINVAL) 
		{
#ifdef DEBUG
			perror ("VIDIOC_QUERYCTRL");
#endif
			return EXIT_FAILURE;
		} 
	} else 
	{
		control.value = value;
		if (-1 == ioctl (dev, VIDIOC_S_CTRL, &control)) {
#ifdef DEBUG
			perror ("VIDIOC_S_CTRL");
#endif
			return EXIT_FAILURE;
        	}
	}
	return EXIT_SUCCESS;
}

int Webcam::currentWidth()
{
	return (int) fmt.fmt.pix.width;
}

int Webcam::currentHeight()
{
	return (int) fmt.fmt.pix.height;
}

int Webcam::currentPixelFormat()
{
	return (int) fmt.fmt.pix.pixelformat;
}

int Webcam::defaultCtrlVal(unsigned int control)
{
	struct v4l2_queryctrl queryctrl;
	char *ctrl;
	
	if(!isOpened)
	{
		return -1;
	}
/*
 * ctrl values :
 * 	0 : Saturation
 * 	1 : Power line Frequency (néons)
 * 	2 : Brightness
 * 	3 : Contrast
 * 	4 : Sharpness
 */
	
	memset(&queryctrl, 0, sizeof queryctrl);
	switch(control){
		case 0 : {
			ctrl = "Saturation";
			queryctrl.id = V4L2_CID_SATURATION;
			break;
		}
		case 1 : {
			ctrl = "Powerline Frequecy";
			queryctrl.id = V4L2_CID_POWER_LINE_FREQUENCY;
			break;
		}
		case 2 : {
			ctrl = "Brightness";
			queryctrl.id = V4L2_CID_BRIGHTNESS;
			break;
		}
		case 3 : {
			ctrl = "Contrast";
			queryctrl.id = V4L2_CID_CONTRAST;
			break;
		}
		case 4 : {
			ctrl = "Sharpness";
			queryctrl.id = V4L2_CID_SHARPNESS;
			break;
		}
	}

char str[128];
	if (-1 == ioctl(dev, VIDIOC_QUERYCTRL, &queryctrl))
	{
		strcpy(str, "Unable to set control ");
		KError(strcat(str, ctrl), errno);
		return EXIT_FAILURE;
	}

	return (unsigned int) queryctrl.default_value;
}

