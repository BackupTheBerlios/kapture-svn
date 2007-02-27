#ifndef MOUSELABEL_H
#define MOUSELABEL_H

#include <QLabel>

class MouseLabel : public QLabel
{
public:
	MouseLabel(QWidget *parent);
	int posCurX;
	int posCurY;
	
protected:
	void mouseMoveEvent(QMouseEvent *e);
		
};

#endif // MOUSELABEL_H

