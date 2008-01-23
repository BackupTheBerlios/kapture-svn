#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QProgressBar>
#include <QLabel>

#include "presence.h"
#include "emoticons.h"
#include "chatwin.h"
#include "jid.h"
#include "vcard.h"
#include "filetransferwidget.h"

class Contact : public QObject
{
	Q_OBJECT
public:
	Contact(const QString&);
	Contact(const QString&, const QString&);
	Contact(const char*);
	~Contact();
	
	void newMessage(const QString& /*Message*/); //not a slot ???

	void setPresence(const Presence&);
	void setResource(QString&);
	void setFeatures(QStringList&);
	
	void startChat();
	bool isAvailable(); // available or unavailable.
	VCard *vCard() const;
	Jid *jid; // must go in private, a method to get it.
	void setTranferFileState(QString, int);
	void setEmoticons(Emoticons*);
	QString show() const {return presence->show();};

private:
	Contact();
	ChatWin *chatWin; 
	bool isChatting;
	QStringList features;
	VCard *vcard;
	
	QList<FileTransferWidget*> transferList;
	bool done;
	Emoticons *e;
	QString showToPretty(const QString&);
	Presence *presence;
	int newMessages;

public slots:
	void messageToSend(QString message);
	void sendFile();
	void slotSendVideo();
	void updateChatWinTitle();

signals:
	void sendMessage(QString&, QString&);
	void sendFileSignal(QString&);
	void sendVideo(QString&);
};

#endif //CONTACT_H
