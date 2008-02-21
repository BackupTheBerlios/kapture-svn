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
#include <QFileDialog>
#include <QFile>

#include "client.h"
#include "presence.h"
#include "message.h"
#include "profile.h"

#define XMLNS_SI "http://jabber.org/protocol/si"
#define XMLNS_FILETRANSFER "http://jabber.org/protocol/si/profile/file-transfer"

Client::Client(Jid &jid, QString server, QString port)
{
	j = jid;
	pS = server;
	p = port.isEmpty() ? 5222 : port.toInt();
}

Client::Client()
{
	
}

Client::~Client()
{
	delete xmpp;
	delete task;
}

void Client::authenticate()
{
	if (j.isValid())
		xmpp = new Xmpp(j, pS, p);

	connect(xmpp, SIGNAL(error(Xmpp::ErrorType)), this, SLOT(connectionError(Xmpp::ErrorType)));
	if (r.isEmpty())
		r = "Kapture";
	if (!pass.isEmpty())
		xmpp->auth(pass, r);

	task = new Task();
	ppTask = new PullPresenceTask(task);
	connect(ppTask, SIGNAL(presenceFinished()), this, SLOT(presenceFinished()));
	pmTask = new PullMessageTask(task);
	connect(pmTask, SIGNAL(messageFinished()), this, SLOT(messageFinished()));
	psTask = new PullStreamTask(task, xmpp);
	connect(psTask, SIGNAL(fileTransferIncoming()), this, SLOT(fileTransferIncoming()));
	connect(psTask, SIGNAL(receiveFileReady()), this, SLOT(receiveFileReady()));
	
	connect(xmpp, SIGNAL(connected()), this, SLOT(authFinished()));
	connect(xmpp, SIGNAL(readyRead()), this, SLOT(read()));
}

void Client::connectionError(Xmpp::ErrorType e)
{
	emit error(e);
}

void Client::receiveFileReady()
{
	QString f = psTask->from().full();
	rfTask = new FileTransferTask(task, f, xmpp);
	connect(rfTask, SIGNAL(prcentChanged(Jid&, QString&, int)), this, SIGNAL(prcentChanged(Jid&, QString&, int)));
	rfTask->setFileInfo(psTask->fileName(), psTask->fileSize());
	rfTask->connectToHosts(psTask->streamHosts(), psTask->sid(), psTask->lastId(), psTask->saveFileName());
}

void Client::fileTransferIncoming()
{
/* 
 * FIXME: That shouldn't be there.
 */
	ifd = new IncomingFileDialog();
	ifd->show();

	ifd->setFrom(psTask->from());
	ifd->setFileName(psTask->fileName());
	ifd->setFileSize(psTask->fileSize());
	ifd->setDesc(psTask->fileDesc());
	connect(ifd, SIGNAL(agree()), this, SLOT(ftAgree()));
	connect(ifd, SIGNAL(decline()), this, SLOT(ftDecline()));
}

void Client::ftAgree()
{
	psTask->ftAgree(ifd->fileName(), ifd->from(), ifd->saveFileName());
	ifd->close();
	delete ifd;
	//psTask->reset();
}

void Client::ftDecline()
{
	psTask->ftDecline(ifd->fileName(), ifd->from());
	ifd->close();
	delete ifd;
}

void Client::authFinished()
{
	j.setResource(xmpp->getResource());
	emit connected();
}


void Client::setResource(const QString& resource)
{
	r = resource;
}

void Client::setJid(const Jid& jid)
{
	j = jid;
}

void Client::setPassword(const QString& password)
{
	pass = password;
}
bool Client::isSecured() const
{
	return xmpp->isSecured();
}

void Client::sendMessage(QString& to, QString& message)
{
	// FIXME:Xml tree should be created here, the method sendMessage() should not exist ix Xmpp class.
	/*Message(const Jid& from,
		const Jid& to,
		const QString& message,
		const QString& type,
		const QString& subject,
		const QString& thread);*/
	
	mTask = new MessageTask(task);
	QString type = "chat";
	QString subject = "";
	QString thread = "";
	Message m(j, Jid(to), message, type, subject, thread);
	mTask->sendMessage(xmpp, m);
}

bool Client::noStanza() const
{
	return !xmpp->stanzaAvailable();
}

Stanza *Client::getFirstStanza()
{
	return xmpp->getFirstStanza();
}

void Client::getRoster()
{
	rTask = new RosterTask(xmpp, task);
	connect(rTask, SIGNAL(finished()), this, SLOT(rosterFinished()));
	rTask->getRoster(j);
}

void Client::rosterFinished()
{
	emit rosterReady(rTask->roster());
	//disconnect(rTask);
	delete rTask;
	rTask = new RosterTask(xmpp, task);
	connect(rTask, SIGNAL(finished()), this, SLOT(slotUpdateItem()));
}

void Client::slotUpdateItem()
{
	printf("[CLIENT] Should add an item.\n");
	emit signalUpdateItem(rTask->roster().contactList().at(0));
}

void Client::setInitialPresence(QString& show, QString& status, QString& type)
{
	printf("[CLIENT] setInitialPresence()\n");
	pTask = new PresenceTask(xmpp, task);

	//QString type = "";
	connect(pTask, SIGNAL(finished()), this, SLOT(setPresenceFinished()));
	pTask->setPresence(show, status, type);
}

void Client::setPresence(const QString& show, const QString& status)
{
	printf("[CLIENT] setPresence()\n");
	pTask = new PresenceTask(xmpp, task);
	connect(pTask, SIGNAL(finished()), this, SLOT(setPresenceFinished()));
	QString type = "";
	pTask->setPresence(show, status, type);
}

/*
 * setInitialPresence finished SIGNAL is connected to this SLOT.
 * Everytime the presence of this resource changes, this SLOT should
 * be called.
 */
void Client::setPresenceFinished()
{
	task->removeChild(pTask);
	delete pTask;
}

void Client::presenceFinished()
{
	Presence p = ppTask->getPresence();
	emit presenceReady(p);
}

// Sends authorization to "to" so "to" can see me.
void Client::addAuthFor(const QString& to)
{
	subTask = new PresenceTask(xmpp, task);
	subTask->setSubscription(to, "subscribed");
	task->removeChild(subTask);
	delete subTask;
}

// Removes authorization to "to" so "to" cannot see me.
void Client::removeAuthFor(const QString& to)
{
	subTask = new PresenceTask(xmpp, task);
	subTask->setSubscription(to, "unsubscribed");
	task->removeChild(subTask);
	delete subTask;
}

void Client::requestAuthFor(const QString& to)
{
	subTask = new PresenceTask(xmpp, task);
	subTask->setSubscription(to, "subscribe");
	connect(subTask, SIGNAL(subApproved()), SLOT(subApproved()));
	connect(subTask, SIGNAL(subRefused()), SLOT(subRefused()));
}

void Client::subApproved()
{
	printf("Client::subApproved()\n");
}

void Client::subRefused()
{
	printf("Client::subRefused()\n");
}

void Client::messageFinished()
{
	Message m = pmTask->getMessage();
	emit messageReady(m);
}

void Client::read()
{
	while (xmpp->stanzaAvailable())
	{
		printf("Read next Stanza\n");
		Stanza *s = xmpp->getFirstStanza();
		printf("Client:: : %s\n", s->from().full().toLatin1().constData());
		task->processStanza(*s);
	}
}

void Client::sendFile(QString& to)
{
	sTask = new StreamTask(task, xmpp, to);
	connect(sTask, SIGNAL(error(int, const QString&)), this, SLOT(streamTaskError(int, const QString&)));
	sTask->discoInfo();
	connect(sTask, SIGNAL(infoDone()), this, SLOT(slotInfoDone()));
}

void Client::streamTaskError(int errCode, const QString&)
{
	switch (errCode)
	{
	case 1:
		emit streamError(Unknown);
		printf("Unknown error\n");
		break;
	case 403:
		emit streamError(Declined);
		printf("User Declined sending invitation.\n");
		break;
	}
	task->removeChild(sTask);
	delete sTask;
}

void Client::slotInfoDone()
{
	if (sTask->supports(XMLNS_SI) && sTask->supports(XMLNS_FILETRANSFER))
	{
		//Ask file here (show file open dialog).
		//QFileDialog *fileDialog = new QFileDialog(this);
		fileName = QFileDialog::getOpenFileName(0,
		         tr("Send File"), QDir::homePath(), tr("All Files (*.*)"));
		f = new QFile(fileName);
		if (f->exists())
		{
			sTask->initStream(*f);
			connect(sTask, SIGNAL(finished()), this, SLOT(transferFile()));
		}
		else
		{
			printf("File Transfer Cancelled\n");
			task->removeChild(sTask);
			delete sTask;
		}
	}
}

void Client::transferFile()
{
	// TODO:Should manage more than 1 transfer at a time.
	sfTask = new FileTransferTask(task, sTask->toJid(), xmpp);
	sfTask->start(sTask->negProfile(), sTask->sid(), fileName, sTask->proxies(), sTask->ips(), sTask->ports());
	connect(sfTask, SIGNAL(prcentChanged(Jid&, QString&, int)), this, SIGNAL(prcentChanged(Jid&, QString&, int)));
	connect(sfTask, SIGNAL(finished()), this, SLOT(transferFinished()));
	connect(sfTask, SIGNAL(notConnected()), this, SLOT(notConnected()));

	task->removeChild(sTask);
	delete sTask;
}

void Client::notConnected()
{
	printf("Unable to connect to the target.\n");
	Jid to = sTask->toJid();
	task->removeChild(sfTask);
	delete sfTask;
	task->removeChild(sTask);
	delete sTask;
}

void Client::transferFinished()
{
	//emit transferTerminated;
	task->removeChild(sfTask);
	delete sfTask;
}

void Client::sendVideo(const QString& to)
{
	svTask = new JingleTask(task, xmpp);
	svTask->initiate(Jid(to));
}

void Client::addItem(const Jid& jid, const QString& name, const QString& /*group*/)
{
	rTask->addItem(jid, name);
}
