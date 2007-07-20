#ifndef MOUSEROSTERTABLEVIEW_H
#define MOUSEROSTERTABLEVIEW_H

#include <QTableView>

class MouseRosterTableView : public QTableView
{
	Q_OBJECT
public:
	MouseRosterTableView(QWidget *parent);
signals:
	void doubleClicked(QString to);

protected:
	void mouseDoubleClickEvent(QMouseEvent *e);
};



#endif// MOUSEROSTERTABLEVIEW_H
