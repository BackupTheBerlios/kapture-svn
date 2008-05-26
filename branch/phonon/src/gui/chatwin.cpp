/*
 *      chatwin.cpp -- Kapture
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

#include <QAbstractScrollArea>
#include <QScrollBar>
#include <QCloseEvent>
#include <QLabel>
#include <QDate>
#include <QTime>

#include "chatwin.h"
#include "emoticons.h"

ChatWin::ChatWin()
{
	ui.setupUi(this);
	connect(ui.sendBtn, SIGNAL(clicked()), this, SLOT(message()));
	connect(ui.sendFileBtn, SIGNAL(clicked()), this, SIGNAL(sendFile()));
	connect(ui.videoBtn, SIGNAL(clicked()), this, SLOT(slotSendVideo()));
	ui.sendBtn->setEnabled(false);
	connect(ui.messageLine, SIGNAL(textChanged(QString)), this, SLOT(composing(QString)));
	connect(ui.messageLine, SIGNAL(returnPressed()), this, SLOT(message()));
}

ChatWin::~ChatWin()
{
}

void ChatWin::slotSendVideo()
{
	videoWidget = new VideoWidget();
	if (!videoWidget->initCamera())
	{
		writeEvent("No camera found.", Error);
		delete videoWidget;
		return;
	}
	writeEvent("Starting Video-Chat.");
	QFrame *line = new QFrame(this);
	line->setFrameShape(QFrame::VLine);
	line->setFrameShadow(QFrame::Sunken);
	QVBoxLayout *vbox = new QVBoxLayout();
	vbox->addWidget(videoWidget);
	vbox->addStretch();
	ui.hboxLayout->addWidget(line);
	ui.hboxLayout->addLayout(vbox);
	videoWidget->play();
	emit sendVideo();
}

void ChatWin::message()
{
	if (ui.messageLine->text() != "")
	{
		emit sendMessage(ui.messageLine->text());
	}
}

void ChatWin::composing(QString text)
{
	if (text.length() > 0)
		ui.sendBtn->setEnabled(true);
	else
		ui.sendBtn->setEnabled(false);
	/*
	 * has to tell the server that client is composing
	 */
}

void ChatWin::writeEvent(const QString& event, EventType et)
{
	if (et == Error)
	{
		ui.discutionText->insertHtml(
		QString("<body bgcolor=\"#00FF55\"><font color='red'> *** [")
			+ QDate::currentDate().toString() + QString(" ")
			+ QTime::currentTime().toString() + QString("] ")
			+ event
			+ QString("</font></body><br>"));	
	}
	else
	{
		ui.discutionText->insertHtml(
		QString("<body bgcolor=\"#00FF55\"><font color='green'> * [")
			+ QDate::currentDate().toString() + QString(" ")
			+ QTime::currentTime().toString() + QString("] ")
			+ event
			+ QString("</font></body><br>"));	
	}
}

void ChatWin::changeEvent(QEvent* event)
{
	// FIXME : Does not work properly
	if (event->type() == QEvent::WindowStateChange || windowState() == Qt::WindowActive)
	{
		printf("[ChatWin] changeEvent\n");
		emit shown();
	}
}

void ChatWin::closeEvent(QCloseEvent* event)
{
	event->ignore();
	hide();
}
