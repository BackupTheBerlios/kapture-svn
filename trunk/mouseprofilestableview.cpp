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

#include "mouseprofilestableview.h"

MouseProfilesTableView::MouseProfilesTableView(QWidget*)
{
}

void MouseProfilesTableView::mouseReleaseEvent(QMouseEvent*)
{
	if (currentIndex().isValid())
	{
		QString pName = this->model()->index(currentIndex().row(), 0).data().toString();
		emit Clicked(pName);
	}
}
