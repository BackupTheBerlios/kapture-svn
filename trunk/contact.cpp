#include "contact.h"
#include "utils.h"

Contact::Contact(QString j)
{
	jid = new Jid(j);
	isChatting = false;
}

Contact::~Contact()
{

}

void Contact::sendFile()
{
	emit sendFileSignal(jid->toQString());
}

void Contact::newMessage(QString m /*Message*/)
{
	if (!isChatting)
	{
		chatWin = new ChatWin();
		chatWin->setWindowTitle(jid->toQString());
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
		connect(chatWin, SIGNAL(sendFile()), this, SLOT(sendFile()));
	}

	chatWin->ui.discutionText->insertHtml(QString("<font color='red'>%1 says :</font><br>%2<br>").arg(jid->toQString()).arg(changeEmoticons(m)));
	
	if (!chatWin->isActiveWindow())
	{
		chatWin->activateWindow();
	}
	chatWin->show();
	
	isChatting = true;
}

void Contact::startChat()
{
	if (!isChatting)
	{
		chatWin = new ChatWin();
		chatWin->setWindowTitle(jid->toQString());
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
		connect(chatWin, SIGNAL(sendFile()), this, SLOT(sendFile()));
		isChatting = true;
	}
	chatWin->show();
}

void Contact::messageToSend(QString message)
{
	printf("Emit sendMessage from Contact class.\n");
	emit sendMessage(jid->toQString(), message);
}

void Contact::setPresence(QString status, QString type)
{
	if (isChatting)
	{
		if (presence.type != type)
			chatWin->ui.discutionText->insertHtml(QString("<font color='green'> * %1 is now %2</font><br>").arg(jid->toQString()).arg(type == "available" ? "online" : "offline"));
		else
		{
			if (presence.status != status)
			{
				chatWin->ui.discutionText->insertHtml(QString("<font color='green'> * %1 is now %2</font><br>").arg(jid->toQString()).arg(status));
			}
		}
	}
	
	presence.status = status;
	presence.type = type;
}

void Contact::setResource(QString r)
{
	jid->setResource(r);
}
