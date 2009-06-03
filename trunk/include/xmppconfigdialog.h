#ifndef XMPPCONFIGDIALOG_H
#define XMPPCONFIGDIALOG_H

#include "ui_xmppconfigdialog.h"
#include "profile.h"

class WebcamConfigWidget;
class Xmpp;
class XmppReg;
class ProfileModel;
class Config;
class Task;
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
	void read();
	void registerError();
	void tabChanged(int);

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
	void initWebcam();
	void stopWebcam();
	Task *task;
	Xmpp *xmpp;
	WebcamConfigWidget *wcw;
};

#endif
