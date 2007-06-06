#include "chatwin.h"

ChatWin::ChatWin()
{
	ui.setupUi(this);
	connect(ui.sendBtn, SIGNAL(clicked()), this, SLOT(message()));
}

ChatWin::~ChatWin()
{

}

void ChatWin::message()
{
	emit sendMessage(cNode, ui.messageLine->text());
	ui.discutionText->insertHtml(QString("<font color='blue'>You said : </font><br>%1<br>").arg(ui.messageLine->text()));
	ui.messageLine->clear();
}

void ChatWin::setContactNode(QString n)
{
	cNode = n; // Todo : I must split adress and ressource.
	
	/*
	 * When I start a chat, I do not have the ressource of the contact.
	 * I have it only when he answers. So, I must split those 2 things
	 * to be able to assign the ressource only after having the first
	 * response instead of creating a new window.
	 */

	setWindowTitle(cNode);
}

QString ChatWin::contactNode()
{
	return cNode;
}

void ChatWin::setContactResource(QString r)
{
	if (!hasResource)
		cResource = r;
}

QString ChatWin::contactResource()
{
	return cResource;
}
