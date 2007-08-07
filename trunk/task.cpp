#include "task.h"


Task::Task(Task *parent)
{
	if (parent)
		parent->appendTask(this);
}

Task::~Task()
{

}

void Task::appendTask(Task* t)
{
	taskList << t;
}

void Task::processStanza(const Stanza& s)
{
	printf("Task:: : %s\n", s.from().full().toLatin1().constData());
	processed = false;
	printf("Count = %d\n", taskList.count());
	for (int i = 0; i < taskList.count(); i++)
	{
		printf("void Task::processStanza(const Stanza& s) : Kind = %s\n", s.kind() == Stanza::Presence ? "Presence" : "NoPresence");
		if (taskList[i]->canProcess(s))
		{
			printf("Ok, processing...\n");
			taskList[i]->processStanza(s);
			processed = true;
			break;
		}
	}
	if (!processed)
	{
		printf("OO Must process that !\n");
	}
}

void Task::removeChild(Task* childTask)
{
	printf("Count = %d\n", taskList.count());
	for (int i = 0; i < taskList.count(); i++)
	{
		if (taskList[i] == childTask)
		{
			printf("Ok, removing...\n");
			taskList.removeAt(i);
			break;
		}
	}

}
