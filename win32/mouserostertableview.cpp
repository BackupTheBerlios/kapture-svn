#include <QMouseEvent>
#include "mouserostertableview.h"


MouseRosterTableView::MouseRosterTableView(QWidget *parent)
	:QTableView(parent)
{

}

void MouseRosterTableView::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (currentIndex().isValid())
	{
		//QString to = currentIndex().data().toString().toLatin1().constData();
		QString to = this->model()->index(currentIndex().row(), 1).data().toString();
		printf("User : %s\n", currentIndex().data().toString().toLatin1().constData());
		emit doubleClicked(to);
	}
}

