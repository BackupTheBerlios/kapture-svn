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
	void mError(int ret);

public slots:
	void showJabberWin();
private:
	Ui::kaptureWin ui;
	XmppWin *xw;

	void closeEvent(QCloseEvent *event);
	bool xmppWinCreated;
};

#endif // KAPTUREWIN_H

