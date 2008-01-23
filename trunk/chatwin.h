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
	void composing(QString);

signals:
	void sendMessage(QString message);
	void sendFile();
	void sendVideo();
private:
	void focusInEvent(QFocusEvent* event);

};
#endif //CHATWIN_H
