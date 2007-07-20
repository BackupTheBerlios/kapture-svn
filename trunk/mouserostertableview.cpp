#include <QMouseEvent>
#include "mouserostertableview.h"
#include "contact.h"

MouseRosterTableView::MouseRosterTableView(QWidget *parent)
	:QTableView(parent)
{

}

void MouseRosterTableView::mouseDoubleClickEvent(QMouseEvent *e)
{
	if (currentIndex().isValid())
	{
		QString to = this->model()->data(currentIndex(), Qt::WhatsThisRole).toString();
		printf("User : %s\n", to.toLatin1().constData());
		emit doubleClicked(to);
	}
}

