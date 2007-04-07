/*
 *      mouselabel.cpp -- Kapture
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
#include <QMouseEvent>
#include "mouselabel.h"

MouseLabel::MouseLabel(QWidget *parent)
	: QLabel(parent)
{
	posCurX = 8;
	posCurY = 8;
}

void MouseLabel::mouseMoveEvent(QMouseEvent *e)
{
//	printf(" * Position : (%d,%d) (%d,%d)\n", e->x(), e->y(), e->globalX(), e->globalY());
	posCurX = e->x();	
	posCurY = e->y();	
}

