#ifndef MOUSEROSTERTABLEVIEW_H
#define MOUSEROSTERTABLEVIEW_H

#include <QTableView>

class MouseRosterTableView : public QTableView
{
	Q_OBJECT
public:
	MouseRosterTableView(QWidget *parent);
signals:
	void doubleClicked(const QString& to);
	void leftClick(const QString&, const QPoint&);

protected:
	void mouseDoubleClickEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
};



#endif// MOUSEROSTERTABLEVIEW_H
