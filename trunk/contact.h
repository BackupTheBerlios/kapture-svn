#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>

#include "chatwin.h"
#include "jid.h"

class Contact : public QObject
{
	Q_OBJECT
public:
	Contact(QString j);
	~Contact();
	void newMessage(QString m /*Message*/);
	Jid *jid;
	void setPresence(QString status, QString show);
	void startChat();

private:
	ChatWin *chatWin; 
	bool isChatting;
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
