#ifndef INCOMINGFILEDIALOG_H
#define INCOMINGFILEDIALOG_H

#include "ui_incomingfiledialog.h"
#include "jid.h"

class IncomingFileDialog : public QDialog
{
	Q_OBJECT
public:
	IncomingFileDialog();
	~IncomingFileDialog();
	void setFrom(const Jid&);
	void setFileName(const QString&);
	void setFileSize(int);
	void setDesc(const QString&);
	QString fileName() const;
	QString saveFileName() const;
	Jid from() const;

signals:
	void agree();
	void decline();
private:
	Ui::incomingFileDialog ui;
	Jid f;
};

#endif
