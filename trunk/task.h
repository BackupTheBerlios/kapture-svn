#ifndef TASK_H
#define TASK_H

#include <QObject>
#include <QDomDocument>
#include <QList>
#include "xmpp.h"

class Task : public QObject
{
	Q_OBJECT
public:
	Task(Task *parent = 0);
	~Task();
	virtual bool canProcess(const Stanza&) const {return false;};
	virtual void processStanza(const Stanza&);
	void removeChild(Task* childTask);
signals:
	void finished();

private:
	void appendTask(Task* t);
	bool processed;
	QList<Task*> taskList;
};

#endif
