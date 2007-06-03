#ifndef MOUSETABLEVIEW_H
#define MOUSETABLEVIEW_H

#include <QTableView>

class MouseTableView : public QTableView
{
	Q_OBJECT
public:
	MouseTableView(QWidget *parent);
signals:
	void doubleClicked(QString to);

protected:
	void mouseDoubleClickEvent(QMouseEvent *e);
};



#endif// MOUSETABLEVIEW_H
