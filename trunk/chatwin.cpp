#include "chatwin.h"

ChatWin::ChatWin()
{
	ui.setupUi(this);
	connect(ui.sendBtn, SIGNAL(clicked()), this, SLOT(message()));
	hasResource = false;
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
		if(hasResource)
			emit sendMessage(cNode + "/" + cResource, ui.messageLine->text());
		else
			emit sendMessage(cNode, ui.messageLine->text());

		ui.discutionText->insertHtml(QString("<font color='blue'>You said : </font><br>%1<br>").arg(ui.messageLine->text()));
		ui.messageLine->clear();
		ui.sendBtn->setEnabled(false);
	}
}

void ChatWin::setContactNode(QString n)
{
	cNode = n;
	setWindowTitle(cNode);
}

QString ChatWin::contactNode()
{
	return cNode;
}

void ChatWin::setContactResource(QString r)
{
	if (!hasResource)
	{
		cResource = r;
		setWindowTitle(cNode + '/' + cResource);
	}
}

QString ChatWin::contactResource()
{
	return cResource;
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

