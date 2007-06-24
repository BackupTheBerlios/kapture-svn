#ifndef CONTACT_H
#define CONTACT_H
#include <QObject>
#include "chatwin.h"

class Contact : public QObject
{
	Q_OBJECT
public:
	Contact(QString j);
	~Contact();
	void newMessage(QString m /*Message*/);
	void setResource(QString r);
	void setJid(QString j);
	QString getJid();
	void setPresence(QString status, QString show);
	void startChat();
	QString getResource();

private:
	ChatWin *chatWin; 
	bool isChatting;
	QString jid; //should be Jid jid;
	QString resource;
	struct Presence
	{
		QString status; // Emergency away,...
		QString show; // dnd, away,...
		QString type; //Offline or online

	} presence;
public slots:
	void messageToSend(QString message);
signals:
	void sendMessage(QString to, QString message);
};

#endif //CONTACT_H
