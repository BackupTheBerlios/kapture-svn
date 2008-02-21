#ifndef MOUSEROSTERTABLEVIEW_H
#define MOUSEROSTERTABLEVIEW_H

#include <QTableView>
#include "jid.h"

class MouseRosterTableView : public QTableView
{
	Q_OBJECT
public:
	MouseRosterTableView(QWidget *parent);
signals:
	void doubleClicked(const Jid& to);
	void rightClick(const Jid&, const QPoint&);

protected:
	void mouseDoubleClickEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
};



#endif// MOUSEROSTERTABLEVIEW_H
