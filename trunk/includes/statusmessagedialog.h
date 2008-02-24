
#ifndef STATUSMESSAGEDIALOG_H
#define STATUSMESSAGEDIALOG_H
#include "ui_statusmessagedialog.h"

class StatusMessageDialog : public QDialog
{
	Q_OBJECT
public:
	StatusMessageDialog();
	~StatusMessageDialog();
public slots:
	void accept();
signals:
	void ok();
private:
	Ui::statusMessageDialog ui;
};

#endif
