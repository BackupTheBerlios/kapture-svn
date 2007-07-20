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
public slots:
	void selectChange(QString);
	void add();
	void del();
signals:
	void accepted();
private:
	Ui::xmppConfigDialog ui;
	QList<Profile> profiles;
	Config *conf;
	ProfileModel *model;
	QString selectedProfile;
};

#endif
