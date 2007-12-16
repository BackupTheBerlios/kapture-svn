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

#include "chatwin.h"
#include "emoticons.h"
#include <QAbstractScrollArea>
#include <QScrollBar>

ChatWin::ChatWin()
{
	ui.setupUi(this);
	connect(ui.sendBtn, SIGNAL(clicked()), this, SLOT(message()));
	connect(ui.sendFileBtn, SIGNAL(clicked()), this, SIGNAL(sendFile()));
	connect(ui.videoBtn, SIGNAL(clicked()), this, SIGNAL(sendVideo()));
	ui.sendBtn->setEnabled(false);
	connect(ui.messageLine, SIGNAL(textChanged(QString)), this, SLOT(composing(QString)));
	connect(ui.messageLine, SIGNAL(returnPressed()), this, SLOT(message()));
}

ChatWin::~ChatWin()
{

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

