/*
 *      Kapture
 *
 *      Copyright (C) 2006-2007
 *          Detlev Casanova (detlev.casanova@gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */
#include <QLabel>

#include "contact.h"
#include "utils.h"

Contact::Contact(const QString& j)
{
	jid = new Jid(j);
	isChatting = false;
	vcard = new VCard();
	done = false;
}

Contact::Contact(const QString &j, const QString &n)
{
	jid = new Jid(j);
	isChatting = false;
	vcard = new VCard();
	vcard->setNickname(n);
	done = false;
}

Contact::Contact(const char *j)
{
	jid = new Jid(j);
	isChatting = false;
	vcard = new VCard();
	done = false;
}

Contact::Contact()
{
	done = false;
}

Contact::~Contact()
{

}

void Contact::sendFile()
{
	QString to = jid->full();
	emit sendFileSignal(to);
}

void Contact::newMessage(const QString &m /*Message*/)
{
	if (!isChatting)
	{
		chatWin = new ChatWin();
		chatWin->setWindowTitle(jid->full());
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
		connect(chatWin, SIGNAL(sendFile()), this, SLOT(sendFile()));
	}

	chatWin->ui.discutionText->insertHtml(QString("<font color='red'>%1 says :</font><br>%2<br>").arg(vcard->nickname() == "" ? jid->full() : vcard->nickname()).arg(changeEmoticons(m)));
	
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
		chatWin->setWindowTitle(jid->full());
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
		connect(chatWin, SIGNAL(sendFile()), this, SLOT(sendFile()));
		isChatting = true;
	}
	chatWin->show();
}

void Contact::messageToSend(QString message)
{
	printf("Emit sendMessage from Contact class. to = %s\n", jid->full().toLatin1().constData());
	QString to = jid->full();
	emit sendMessage(to, message);
}

void Contact::setPresence(QString& status, QString& type)
{
	if (isChatting)
	{
		if (presence.type != type)
			chatWin->ui.discutionText->insertHtml(QString("<font color='green'> * %1 is now %2</font><br>").arg(jid->full()).arg(type == "available" ? "online" : "offline"));
		else
		{
			if (presence.status != status)
			{
				chatWin->ui.discutionText->insertHtml(QString("<font color='green'> * %1 is now %2</font><br>").arg(jid->full()).arg(status));
			}
		}
	}
	
	presence.status = status;
	presence.type = type;
	printf("Contact has a new nickname : %s\n", vcard->nickname().toLatin1().constData());
}

void Contact::setResource(QString& r)
{
	jid->setResource(r);
}

bool Contact::isAvailable()
{
	if (presence.type == "available")
		return true;
	else
		return false;
}

void Contact::setFeatures(QStringList &c)
{
	features = c;
}

VCard *Contact::vCard() const
{
	return vcard;
}

void Contact::setTranferFileState(QString fileName, int prc)
{
	printf("prc = %d\n", prc);
	if (prc == 0 && !done)
	{
		/*printf("It's hapening now.\n");
		//Add the transfer bar
		fileTransferBar = new QProgressBar(chatWin);
		fileTransferBar->setAlignment(Qt::AlignHorizontal_Mask);
		fileTransferBar->setRange(0, 100);

		QLabel *lab = new QLabel();
		lab->setScaledContents(true);
		fileName.truncate(20);
		lab->setText(fileName + "..." + " : ");

		QHBoxLayout *hboxlayout = new QHBoxLayout(chatWin); // a QHBoxLayout should already be present in the window at the right place.
		hboxlayout->addWidget(lab);
		hboxlayout->addWidget(fileTransferBar);

		chatWin->ui.vboxLayout->insertLayout(1, hboxlayout);
		done = true;
	}
	fileTransferBar->setValue(prc);
	if (prc == 100)
	{
		delete fileTransferBar;
		//delete lab;
		//delete hboxlayout;*/
	}
}

