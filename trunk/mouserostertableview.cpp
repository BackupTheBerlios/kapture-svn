/*
 *      Kapture
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
#include "mouserostertableview.h"
#include "contact.h"

MouseRosterTableView::MouseRosterTableView(QWidget *parent)
	:QTableView(parent)
{

}

void MouseRosterTableView::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (currentIndex().isValid() && (e->button() == Qt::LeftButton))
	{
		QString to = this->model()->data(currentIndex(), Qt::WhatsThisRole).toString();
		//printf("User : %s\n", to.toLatin1().constData());
		emit doubleClicked(Jid(to));
	}
}

void MouseRosterTableView::mouseReleaseEvent(QMouseEvent *e)
{
	if (e->button() == Qt::RightButton)
	{
		printf("Current Index = %s\n", currentIndex().isValid() ? "Valid" : "Not Valid");
		if (currentIndex().isValid())
		{
			QString to = this->model()->data(currentIndex(), Qt::WhatsThisRole).toString();
			emit rightClick(Jid(to), e->globalPos());
		}
	}
}
