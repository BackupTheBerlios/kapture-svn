/*
 *      mainframewin.h -- Kapture
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

#ifndef MAINFRAMEWIN_H
#define MAINFRAMEWIN_H

#include <QMainWindow>
#include <QTimer>
#include <unistd.h>

#include "ui_mainframewin.h"
#include "webcam.h"

class MainFrameWin : public QMainWindow
{
	Q_OBJECT
public:
	MainFrameWin();
	~MainFrameWin();
	int posCurX;
	int posCurY;
	Ui::mainFrameWin ui;

signals:
	void turnRightEvent();
	void turnLeftEvent();
	void turnUpEvent();
	void turnDownEvent();
};

#endif // MAINFRAMEWIN_H

