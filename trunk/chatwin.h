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
	void setContactNode(QString n);
	QString contactNode();
	void setContactResource(QString r);
	QString contactResource();
	bool hasResource;

public slots:
	void message();

private:
	QString cNode; // Contact's Node
	QString cResource; // Contact's Resource

signals:
	void sendMessage(QString to, QString message);


};
#endif //CHATWIN_H
