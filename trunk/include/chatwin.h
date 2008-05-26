#ifndef CHATWIN_H
#define CHATWIN_H
#include "ui_chatwin.h"
#include "videowidget.h"

class ChatWin : public QWidget
{
	Q_OBJECT
public:
	ChatWin();
	~ChatWin();
	Ui::chatWin ui;
	enum EventType
	{
		Info = 0,
		Error
	};
	void writeEvent(const QString&, ChatWin::EventType et = ChatWin::Info);

public slots:
	void message();
	void composing(QString);
	void slotSendVideo();

signals:
	void sendMessage(QString message);
	void sendFile();
	void sendVideo();
	void shown();
private:
	void changeEvent(QEvent* event);
	void closeEvent(QCloseEvent*);
	VideoWidget *videoWidget;

};
#endif //CHATWIN_H
