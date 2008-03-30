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
#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QDate>
#include <QTime>
#include <QMessageBox>

#include "contact.h"

Contact::Contact(const QString& j)
{
	jid = new Jid(j);
	isChatting = false;
	vcard = new VCard();
	done = false;
	p = new Presence(QString(""), QString(""), QString(""));
	newMessages = 0;
}

Contact::Contact(const QString& j, const QString& n)
{
	jid = new Jid(j);
	isChatting = false;
	vcard = new VCard();
	vcard->setNickname(n);
	done = false;
	p = new Presence(QString(""), QString(""), QString(""));
	newMessages = 0;
}

Contact::Contact(const QString& j, const QString& n, const QString& s)
{
	jid = new Jid(j);
	isChatting = false;
	vcard = new VCard();
	vcard->setNickname(n);
	done = false;
	p = new Presence(QString(""), QString(""), QString(""));
	newMessages = 0;
	sub = s;
}

Contact::Contact(const char *j)
{
	jid = new Jid(j);
	isChatting = false;
	vcard = new VCard();
	done = false;
	p = new Presence(QString(""), QString(""), QString(""));
	newMessages = 0;
}

Contact::Contact()
{
	done = false;
	p = new Presence(QString(""), QString(""), QString(""));
	newMessages = 0;
}

Contact::~Contact()
{

}

void Contact::sendFile()
{
	QString to = jid->full();
	emit sendFileSignal(to);
}

bool Contact::askForJingleStart(const Jid& from, const QString& /*type*/)
{
	if (!isChatting)
	{
		chatWin = new ChatWin();
		chatWin->setWindowTitle(jid->full());
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
		connect(chatWin, SIGNAL(sendFile()), this, SLOT(sendFile()));
		connect(chatWin, SIGNAL(sendVideo()), this, SLOT(slotSendVideo()));
		connect(chatWin, SIGNAL(shown()), this, SLOT(updateChatWinTitle()));
		isChatting = true;
	}
	chatWin->show();
	int ret = QMessageBox::question(chatWin,
					QString("Jingle Video"),
					QString("The contact ") + 
					from.full() + 
					QString(" wants to start a Jingle video chat with you (should be send and receive...)\nDo you agree ?"), 
					QMessageBox::Yes | QMessageBox::No);
	if (ret != QMessageBox::Yes)
		return false;
	else
		return true;
}

void Contact::newMessage(const QString &m /*Message*/)
{
	if (!isChatting)
	{
		chatWin = new ChatWin();
		chatWin->setWindowTitle(jid->full());
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
		connect(chatWin, SIGNAL(sendFile()), this, SLOT(sendFile()));
		connect(chatWin, SIGNAL(sendVideo()), this, SLOT(slotSendVideo()));
		connect(chatWin, SIGNAL(shown()), this, SLOT(updateChatWinTitle()));
	}

	chatWin->ui.discutionText->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	chatWin->ui.discutionText->insertHtml(QString("<font color='red'>[")
			+ QDate::currentDate().toString(Qt::LocaleDate) + QString(" ")
			+ QTime::currentTime().toString() + QString("] ")
			+ QString("%1 says :</font><br>%2<br>").arg(
				vcard->nickname() == "" ? jid->full() : vcard->nickname()).arg(
					e->changeEmoticons(m)));
	chatWin->ui.discutionText->verticalScrollBar()->setValue(chatWin->ui.discutionText->verticalScrollBar()->maximum());
	
	if (!chatWin->isActiveWindow() && newMessages == 0)
	{
		chatWin->setWindowTitle(QString("^ ") + chatWin->windowTitle());
		newMessages++;
	}
	chatWin->show();
	
	isChatting = true;
}

void Contact::slotSendVideo()
{
	QString to = jid->full();
	emit sendVideo(to);
}

void Contact::startChat()
{
	if (!isChatting)
	{
		chatWin = new ChatWin();
		chatWin->setWindowTitle(jid->full());
		connect(chatWin, SIGNAL(sendMessage(QString)), this, SLOT(messageToSend(QString)));
		connect(chatWin, SIGNAL(sendFile()), this, SLOT(sendFile()));
		connect(chatWin, SIGNAL(sendVideo()), this, SLOT(slotSendVideo()));
		connect(chatWin, SIGNAL(shown()), this, SLOT(updateChatWinTitle()));
		isChatting = true;
	}
	chatWin->show();
}

void Contact::updateChatWinTitle()
{
	newMessages = 0;
	chatWin->setWindowTitle(jid->full());
}

void Contact::messageToSend(QString message)
{
	printf("Emit sendMessage from Contact class. to = %s\n", jid->full().toLatin1().constData());

	chatWin->ui.discutionText->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
	//chatWin->ui.discutionText->insertHtml(e->changeEmoticons(chatWin->ui.messageLine->text()).arg("You"));
	chatWin->ui.discutionText->insertHtml(QString("<font color='blue'>[")
					+ QDate::currentDate().toString(Qt::LocaleDate) + QString(" ")
					+ QTime::currentTime().toString() + QString("] ")
					+ QString("You said : </font><br>%1<br>").arg(e->changeEmoticons(chatWin->ui.messageLine->text())));
	chatWin->ui.discutionText->verticalScrollBar()->setValue(chatWin->ui.discutionText->verticalScrollBar()->maximum());
	chatWin->ui.messageLine->clear();
	chatWin->ui.sendBtn->setEnabled(false);
	
	QString to = jid->full();
	emit sendMessage(to, message);
}

QString Contact::showToPretty(const QString& show)
{
	if (show.toLower() == "dnd")
		return QString("busy");
	if (show.toLower() == "chat")
		return QString("available to chat");
	if (show.toLower() == "away")
		return QString("away");
	if (show.toLower() == "xa")
		return QString("far far away");
	return QString();
}

void Contact::setPresence(const Presence& pr)
{
	/*
	 * Type = [available, unavailable]
	 * Show = [away, chat, dnd, xa]
	 * Status = Text
	 */
	if (isChatting)
	{
		chatWin->ui.discutionText->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
		if (p->type() != pr.type())
			chatWin->ui.discutionText->insertHtml(QString("<font color='green'> * %1 is now %2</font><br>").arg(jid->full()).arg(pr.type() == "available" ? "online" : "offline"));
		else
		{
			if (pr.show() != p->show() && pr.show() != "" && pr.status() != "")
				chatWin->ui.discutionText->insertHtml(
					QString("<body bgcolor=\"#00FF55\"><font color='green'> * [")
					+ QDate::currentDate().toString() + QString(" ")
					+ QTime::currentTime().toString() + QString("] ")
					+ jid->full()
					+ QString(" is now ")
					+ showToPretty(pr.show())
					+ QString(" [")
					+ pr.status()
					+ QString("]</font></body><br>"));
			else
				chatWin->ui.discutionText->insertHtml(
				        QString("<body bgcolor=\"#00FF55\"><font color='green'> * [")
					+ QDate::currentDate().toString() + QString(" ")
					+ QTime::currentTime().toString() + QString("] ")
					+ jid->full()
					+ QString(" is now ")
					+ pr.type() == "available" ? QString("online") : QString("offline")
					+ QString("</font></body><br>"));
		}
		chatWin->ui.discutionText->verticalScrollBar()->setValue(chatWin->ui.discutionText->verticalScrollBar()->maximum());
	}
	
	p->setType(pr.type());
	p->setShow(pr.show());
	p->setStatus(pr.status());
	printf("[CONTACT] Presence : type = %s, show = %s, status %s\n", p->type().toLatin1().constData(),
							       p->show().toLatin1().constData(),
							       p->status().toLatin1().constData());
	printf("[CONTACT] Contact has a new nickname : %s\n", vcard->nickname().toLatin1().constData());
}

void Contact::setResource(QString& r)
{
	jid->setResource(r);
}

bool Contact::isAvailable()
{
	if (p->type() == "available")
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
	//printf("prc = %d\n", prc);
	if (prc == 0 && !done)
	{
		//Add the transfer bar
		
		FileTransferWidget *ftw = new FileTransferWidget(fileName);
		transferList.append(ftw);

		chatWin->ui.vboxLayout->insertLayout(transferList.count(), transferList.last()->box());
		done = true;
	}
	
	for (int i = 0; i < transferList.count(); i++)
	{
		if (transferList.at(i)->fileName() == fileName)
		{
			transferList.at(i)->setPourcentage(prc);
			if (prc == 100)
			{
				chatWin->ui.vboxLayout->removeItem(transferList.at(i)->box());
				delete transferList.at(i);
				transferList.removeAt(i);
				done = false;
			}
			break;
		}
	}
}

void Contact::setEmoticons(Emoticons* emoticons)
{
	e = emoticons;
}

void Contact::setSubscription(const QString& subscription)
{
	sub = subscription;
}
