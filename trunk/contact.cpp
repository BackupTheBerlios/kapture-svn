#include "contact.h"
#include "utils.h"

Contact::Contact(QString j)
{
	jid = j;
	isChatting = false;
	resource = "";
}

Contact::~Contact()
{

}

void Contact::newMessage(QString m /*Message*/)
{
	if (!isChatting)
	{
		chatWin = new ChatWin();
		chatWin->setWindowTitle(jid);
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
	}

	chatWin->ui.discutionText->insertHtml(QString("<font color='red'>%1 says :</font><br>%2<br>").arg(jid).arg(changeEmoticons(m)));
	
	if (!chatWin->isActiveWindow())
	{
		chatWin->activateWindow();
	}
	chatWin->show();
	
	isChatting = true;
}

void Contact::setResource(QString r)
{
	resource = r;
}

QString Contact::getResource()
{
	return resource;
}

void Contact::setJid(QString j)
{
	jid = j;
}

QString Contact::getJid()
{
	return jid;
}

void Contact::startChat()
{
	if (!isChatting)
	{
		chatWin = new ChatWin();
		chatWin->setWindowTitle(jid);
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
		isChatting = true;
	}
	chatWin->show();
}

void Contact::messageToSend(QString message)
{
	printf("Emit sendMessage from Contact class.\n");
	emit sendMessage(resource == "" ? jid : jid + '/' + resource, message);
}

void Contact::setPresence(QString status, QString type)
{
	if (isChatting)
	{
		if (presence.type != type)
			chatWin->ui.discutionText->insertHtml(QString("<font color='green'> * %1 is now %2</font><br>").arg(jid).arg(type == "avaible" ? "online" : "offline"));
		else
		{
			if (presence.status != status)
			{
				chatWin->ui.discutionText->insertHtml(QString("<font color='green'> * %1 is now %2</font><br>").arg(jid).arg(status));
			}
		}
	}
	
	presence.status = status;
	presence.type = type;
}

