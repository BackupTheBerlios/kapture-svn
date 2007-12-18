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
	Contact(const QString &j);
	Contact(const QString &j, const QString &n);
	Contact(const char *j);
	~Contact();
	
	void newMessage(const QString &m /*Message*/); //not a slot ???

	void setPresence(const Presence&);
	void setResource(QString &resource);
	void setFeatures(QStringList &features);
	
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

public slots:
	void messageToSend(QString message);
	void sendFile();
	void slotSendVideo();
signals:
	void sendMessage(QString&, QString&);
	void sendFileSignal(QString&);
	void sendVideo(QString&);
};

#endif //CONTACT_H
