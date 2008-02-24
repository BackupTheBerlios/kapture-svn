/*
 *      mouselabel.h -- Kapture
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
#ifndef MOUSELABEL_H
#define MOUSELABEL_H

#include <QLabel>

class MouseLabel : public QLabel
{
public:
	MouseLabel(QWidget *parent);
	int posCurX;
	int posCurY;
	
protected:
	void mouseMoveEvent(QMouseEvent *e);
		
};

#endif // MOUSELABEL_H

