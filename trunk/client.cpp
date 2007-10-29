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

#define XMLNS_SI "http://jabber.org/protocol/si"
#define XMLNS_FILETRANSFER "http://jabber.org/protocol/si/profile/file-transfer"

Client::Client(Jid &jid, QString server, QString port)
{
	j = jid;
	pS = server;
	p = port.isEmpty() ? 5222 : port.toInt();
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

	connect(xmpp, SIGNAL(error(Xmpp::ErrorType)), this, SIGNAL(error(Xmpp::ErrorType)));
	if (r.isEmpty())
		r = "Kapture";
	if (!pass.isEmpty())
		xmpp->auth(pass, r);

	task = new Task();
	ppTask = new PullPresenceTask(task);
	connect(ppTask, SIGNAL(presenceFinished()), this, SLOT(presenceFinished()));
	pmTask = new PullMessageTask(task);
	connect(pmTask, SIGNAL(messageFinished()), this, SLOT(messageFinished()));
	
	connect(xmpp, SIGNAL(connected()), this, SLOT(authFinished()));
	connect(xmpp, SIGNAL(readyRead()), this, SLOT(read()));
}

void Client::authFinished()
{
	j.setResource(xmpp->getResource());
	emit connected();
}

/*void Client::connected()
{
	printf("Connected !\n");
	emit connected();
}*/

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
	rTask = new RosterTask(task);

	connect(rTask, SIGNAL(finished()), this, SLOT(rosterFinished()));
	rTask->getRoster(xmpp, j);
}

void Client::rosterFinished()
{
	emit rosterReady(rTask->roster());
	task->removeChild(rTask);
	
	delete rTask;
}

void Client::setInitialPresence(QString& show, QString& status)
{
	pTask = new PresenceTask(task);

	QString type = "";
	connect(pTask, SIGNAL(finished()), this, SLOT(setPresenceFinished()));
	pTask->setPresence(xmpp, show, status, type);
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
	sTask->setUseProxy(false);
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
			if (sTask->useProxy())
				sTask->initProxyStream(*f);
			else
				sTask->initStream(*f);
			connect(sTask, SIGNAL(finished()), this, SLOT(transferFile()));
		}
		else
			printf("Unable to negatiate transfer protocol.\n");
	}
}

void Client::transferFile()
{
	// Should manage more than 1 transfer at a time.
	ftTask = new FileTransferTask(task, sTask->toJid(), xmpp);
	ftTask->start(sTask->negProfile(), sTask->sid(), fileName);
	connect(ftTask, SIGNAL(prcentChanged(Jid&, QString&, int)), this, SIGNAL(prcentChanged(Jid&, QString&, int)));
	connect(ftTask, SIGNAL(finished()), this, SLOT(transferFinished()));
	connect(ftTask, SIGNAL(notConnected()), this, SLOT(notConnected()));

	task->removeChild(sTask);
	delete sTask;
}

void Client::notConnected()
{
	printf("Unable to connect to the target, trying with a proxy.\n");
	Jid to = sTask->toJid();
	delete ftTask;
	delete sTask;
	
	sTask = new StreamTask(task, xmpp, to);
	sTask->setUseProxy(true);
	connect(sTask, SIGNAL(error(int, const QString&)), this, SLOT(streamTaskError(int, const QString&)));
	sTask->discoInfo();
	connect(sTask, SIGNAL(infoDone()), this, SLOT(slotInfoDone()));
}

void Client::transferFinished()
{
	//emit transferTerminated;
	task->removeChild(ftTask);
	delete ftTask;
}

/*void Client::processIq(const QDomDocument& d)
{
	printf("process IQ\n");
}


void Stanza::setupIq(QDomElement s)
{
	id = s.attribute("id");
	from = s.attribute("from");
	type = s.attribute("type");

	if (s.hasChildNodes())
		s = s.firstChildElement();
	else
		printf("error ! ");
	
		// !!! DO NOT MODIFY s BEFORE ""s = s.nextSibling().toElement(); !!!
	while (!s.isNull())
	{
		QString tag = s.localName();
		if (tag == "query")
		{
			if (id == "roster_1")
			{
				QDomNodeList items = s.childNodes();
				for (int i = 0; i < items.count(); i++)
				{
					contacts << items.at(i).toElement().attribute("jid");
					nicknames << items.at(i).toElement().attribute("name");
					printf("New Roster contact : %s (subscription : %s)\n", contacts[i].toLatin1().constData(), items.at(i).toElement().attribute("subscription").toLatin1().constData());
				}
			}

			if (s.namespaceURI() == XMLNS_DISCO)
			{
				if (type == "get")
					action = SendDiscoInfo;
				if (type == "result")
				{
					QDomNodeList p = s.childNodes();
					
					for (int i = 0; i < p.count(); i++)
					{
						if (p.at(i).localName() == "feature")
							features << p.at(i).toElement().attribute("var");
					}

					action = ReceivedDiscoInfo;
				}
			}
			else
			{
				printf("Action : None\n");
				action = None;
			}
		}
		
		if (tag == "body")
		{
		}
		
		s = s.nextSibling().toElement();
	}
}
*/

/*void Client::processPresence(const QDomDocument& d)
{
	printf("process Presence\n");
}

void Client::processMessage(const QDomDocument& d)
{
	printf("process Message\n");
}*/
