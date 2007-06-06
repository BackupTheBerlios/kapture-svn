#include <QMouseEvent>
#include "mousetableview.h"


MouseTableView::MouseTableView(QWidget *parent)
	:QTableView(parent)
{

}

void MouseTableView::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (currentIndex().isValid())
	{
		QString to = currentIndex().data().toString().toLatin1().constData();
		printf("User : %s\n", currentIndex().data().toString().toLatin1().constData());
		emit doubleClicked(to);
	}
}

