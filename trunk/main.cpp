#ifdef linux
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QFrame>

#include "kapturewin.h"

int main(int argc, char **argv)
{
	QApplication app(argc, argv);
	KaptureWin *kw = new KaptureWin;
	kw->show();
	app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()) );
	return app.exec();
}
#else
int main(int argc, char **argv)
{
	printf("This is a GNU/Linux only software.\nSorry.");
	return -1;
}
#endif
