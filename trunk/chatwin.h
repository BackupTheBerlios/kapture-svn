#ifndef CHATWIN_H
#define CHATWIN_H
#include "ui_chatwin.h"

class ChatWin : public QWidget
{
	Q_OBJECT
public:
	ChatWin();
	~ChatWin();
	Ui::chatWin ui;
public slots:
	void message();

signals:
	void sendMessage(QString to, QString message);


};
#endif //CHATWIN_H
