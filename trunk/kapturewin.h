/*
 *      kapturewin.cpp -- Kapture
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

#ifndef KAPTUREWIN_H
#define KAPTUREWIN_H

#include <QMainWindow>
#include <QTimer>
#include <unistd.h>

#include "ui_kapturewin.h"
#include "mainframewin.h"
#include "xmppwin.h"
#include "webcam.h"

class KaptureWin : public QMainWindow
{
	Q_OBJECT
public:
	KaptureWin();
	~KaptureWin();
	QPixmap zoomAtCursor(int xCurPos, int yCurPos, QImage imageToZoomIn);
	void mError(int ret);
	int posCurX;
	int posCurY;
	QList<int> formatList;
	QList<QString> formatName;

public slots:
	void getImage();
	void changeSize(const QString & itemSelected);
	void changeFormat(const QString & itemSelected);
	void getDeviceCapabilities();
	void startStopVideo();
	void savePhoto();
	void crStartStop();
	void colorChanged();
	void showColorReplaced();
	void satChanged();
	void brightChanged();
	void contChanged();
	void freqChanged();
	void sharpChanged();
	void keepZoomerTimeOut();
	void showJabberWin();
	int showZoom();
private:
	Ui::kaptureWin ui;
	Webcam *camera;
	MainFrameWin *mfw;
	XmppWin *xw;

	QTimer waitCamera;
	QTimer keepZoomer;
	QImage mainImageSav;
	QImage crImage;
	QImage imageFromCamera;
	QString videoDevice;
	bool otherVideoDevice;
	bool imageSaved;
	bool xmppWinCreated;
	bool isCapturing;
	bool crIsActivated;
	void closeEvent(QCloseEvent*);
	int fctExecuted;
	bool panTiltSupported;
};

#endif // KAPTUREWIN_H

