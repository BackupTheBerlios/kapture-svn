#ifndef XMPPCONFIGDIALOG_H
#define XMPPCONFIGDIALOG_H

#include "ui_xmppconfigdialog.h"
#include "profile.h"
#include "config.h"
#include "profilemodel.h"
#include "mouseprofilestableview.h"
#include "xmppreg.h"
#include "xmpp.h"

class XmppConfigDialog : public QDialog
{
	Q_OBJECT
public:
	XmppConfigDialog();
	~XmppConfigDialog();
public slots:
	void selectChange(QString&);
	void add();
	void regFinished();
	void del();
	void configWebcam();
	void saveConfig();
signals:
	void accepted();
private:
	Ui::xmppConfigDialog ui;
	QList<Profile> profiles;
	Config *conf;
	ProfileModel *model;
	QString selectedProfile;
	XmppReg *xmppReg;
	void addProfile(const Profile&);
	Task *task;
	Xmpp *xmpp;
};

#endif
