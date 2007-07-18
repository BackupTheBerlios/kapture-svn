#ifndef MOUSEPROFILESTABLEVIEW_H
#define MOUSEPROFILESTABLEVIEW_H

#include <QTableView>

class MouseProfilesTableView : public QTableView
{
	Q_OBJECT
public:
	MouseProfilesTableView(QWidget *parent = 0);
signals:
	void Clicked(QString to);

protected:
	void mouseReleaseEvent(QMouseEvent *e);
};

#endif// MOUSEROSTERTABLEVIEW_H
