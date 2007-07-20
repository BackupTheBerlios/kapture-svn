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
	void setPresence(QString status, QString type);
	//TODO : fix the mistakes in the names (Presence type is available or unavailable)
	void startChat();
	void setResource(QString);
	bool isAvailable(); // available or unavailable.
	void setFeatures(QStringList);

private:
	ChatWin *chatWin; 
	bool isChatting;
	struct Presence
	{
		QString status; // Emergency away,...
		QString show; // dnd, away,...
		QString type; //Offline or online

	} presence;
	QStringList features;
public slots:
	void messageToSend(QString message);
	void sendFile();
signals:
	void sendMessage(QString to, QString message);
	void sendFileSignal(QString);
};

#endif //CONTACT_H
