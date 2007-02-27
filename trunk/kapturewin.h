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

public slots:
	void getImage();
	void changeFormat(const QString & itemSelected);
	void getDeviceCapabilities();
	void startStopVideo();
	void savePhoto();
	void crStartStop();
	void colorChanged();
	void showColorReplaced();
	void satChanged();
	void freqChanged();
	void brightChanged();
	void contChanged();
	void sharpChanged();
	void treatFrame();
	int showZoom();
	void keepZoomerTimeOut();
	
private:
	Ui::kaptureWin ui;
	Webcam *camera;
	MainFrameWin *mfw;

	QTimer waitCamera;
	QTimer keepZoomer;
	bool isCapturing;
	bool crIsActivated;
	QImage mainImageSav;
	QImage crImage;
	bool imageSaved;
	QImage imageFromCamera;
	int fctExecuted;
	void closeEvent(QCloseEvent *event);
};

#endif // KAPTUREWIN_H

