#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QProgressBar>
#include <QLabel>

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
	
	void setPresence(QString &status, QString &type); //TODO : fix the mistakes in the names (Presence type is available or unavailable)
	void setResource(QString &resource);
	void setFeatures(QStringList &features);
	
	void startChat();
	bool isAvailable(); // available or unavailable.
	VCard *vCard() const;
	Jid *jid; // must go in private, a method to get it.
	void setTranferFileState(QString, int);
	void setEmoticons(Emoticons*);

private:
	Contact();
	ChatWin *chatWin; 
	bool isChatting;
	struct Presence
	{
		QString status; // Emergency away,...
		QString show; // dnd, away,...
		QString type; //Offline or online

	} presence;
	QStringList features;
	VCard *vcard;
	
	QList<FileTransferWidget*> transferList;
	bool done;
	Emoticons *e;

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
