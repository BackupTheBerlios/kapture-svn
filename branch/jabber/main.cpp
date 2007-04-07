/*
 *      main.cpp -- Kapture
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
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QFrame>

#include "kapturewin.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	KaptureWin *kw = new KaptureWin;
//	kw->show();
	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );
	return app.exec();
}