#include "mouseprofilestableview.h"

MouseProfilesTableView::MouseProfilesTableView(QWidget *parent)
{
}

void MouseProfilesTableView::mouseReleaseEvent(QMouseEvent *e)
{
	if (currentIndex().isValid())
	{
		QString pName = this->model()->index(currentIndex().row(), 0).data().toString();
		emit Clicked(pName);
	}
}
