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

#include <unistd.h>
#include <QCheckBox>
#include <QAbstractButton>
#include <QImageReader>

#include "kapturewin.h"
#include "xmpp.h"

/*
 * This Software is at a developpement state.
 * This is under GNU Public License.
 * You can use, modify or redistribute it.
 *
 * FIXME:
 * 	- Add comments in the code.
 * 	- The timer can be replaced by Dbus (Qt >= 4.2)
 * TODO:
 * 	- Add more goodies (B&W, ...)
 * 	- Add support for more controls (Pan-tilts,...)
 */

KaptureWin::KaptureWin()
	: QMainWindow()
{
	
	xmppWinCreated = false;
	ui.setupUi(this);
	ui.crBox->hide();

	ui.showMfwBtn->setEnabled(false);
	
	ui.comboBoxSize->setEnabled(false);
	
	ui.satManualValueBox->setEnabled(false);
	ui.brightManualValueBox->setEnabled(false);
	ui.contManualValueBox->setEnabled(false);
	ui.freqBox->setEnabled(false);
	ui.sharpManualValueBox->setEnabled(false);

	ui.btnVideo->setEnabled(false);
	ui.btnPhoto->setEnabled(false);
	
	// Xmpp part
	connect(ui.jabberBtn, 	SIGNAL(clicked()), this, SLOT(showJabberWin()));

}

KaptureWin::~KaptureWin()
{

}

void KaptureWin::mError(int ret)
{

}

void KaptureWin::closeEvent(QCloseEvent *event)
{
	printf("\n * Exiting...\n");
	((QApplication*) this->parentWidget())->quit();
}

void KaptureWin::showJabberWin()
{
	if (!xmppWinCreated)
	{
		xw = new XmppWin();
		xmppWinCreated = true;
	}
	xw->show();
}
