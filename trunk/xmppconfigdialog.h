#ifndef XMPPCONFIGDIALOG_H
#define XMPPCONFIGDIALOG_H

#include "ui_xmppconfigdialog.h"
//#include <QDialog>

class XmppConfigDialog : public QDialog
{
	Q_OBJECT
public:
	XmppConfigDialog::XmppConfigDialog();
	XmppConfigDialog::~XmppConfigDialog();
private:
	Ui::xmppConfigDialog ui;
};

#endif
