#ifndef XMPPCONFIGDIALOG_H
#define XMPPCONFIGDIALOG_H

#include "ui_xmppconfigdialog.h"
#include "profile.h"
#include "config.h"
#include "profilemodel.h"
#include "mouseprofilestableview.h"

class XmppConfigDialog : public QDialog
{
	Q_OBJECT
public:
	XmppConfigDialog();
	~XmppConfigDialog();
	ProfileModel *model;
public slots:
	void selectChange(QString);
private:
	Ui::xmppConfigDialog ui;
	QList<Profile> profiles;
};

#endif
