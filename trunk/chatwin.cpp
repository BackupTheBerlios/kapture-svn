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
#include "utils.h"
#include <QAbstractScrollArea>
#include <QScrollBar>

ChatWin::ChatWin()
{
	ui.setupUi(this);
	connect(ui.sendBtn, SIGNAL(clicked()), this, SLOT(message()));
	connect(ui.sendFileBtn, SIGNAL(clicked()), this, SLOT(file()));
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

		ui.discutionText->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
		ui.discutionText->insertHtml(QString("<font color='blue'>You said : </font><br>%1<br>").arg(changeEmoticons(ui.messageLine->text())));
		ui.discutionText->verticalScrollBar()->setValue(ui.discutionText->verticalScrollBar()->maximum());
		ui.messageLine->clear();
		ui.sendBtn->setEnabled(false);
	}
}

void ChatWin::file()
{
	emit sendFile();
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

