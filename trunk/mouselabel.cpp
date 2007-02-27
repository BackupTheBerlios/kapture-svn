#include <QMouseEvent>
#include "mouselabel.h"

MouseLabel::MouseLabel(QWidget *parent)
	: QLabel(parent)
{
	posCurX = 8;
	posCurY = 8;
}

void MouseLabel::mouseMoveEvent(QMouseEvent *e)
{
//	printf(" * Position : (%d,%d) (%d,%d)\n", e->x(), e->y(), e->globalX(), e->globalY());
	posCurX = e->x();	
	posCurY = e->y();	
}

