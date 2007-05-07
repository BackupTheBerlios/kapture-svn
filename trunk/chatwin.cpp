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
	emit sendMessage(windowTitle(), ui.messageLine->text());
	ui.discutionText->insertHtml(QString("<font color='blue'>You said : </font><br>%1<br>").arg(ui.messageLine->text()));
	ui.messageLine->clear();
}
