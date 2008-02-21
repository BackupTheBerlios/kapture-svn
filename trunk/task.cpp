/*
 *      Kapture
 *
 *      Copyright (C) 2006-2007
 *          Detlev Casanova (detlev.casanova@gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

#include "task.h"
#include <cstdlib>
#include <ctime>


Task::Task(Task *parent)
{
	srand((unsigned)time(0));
	if (parent)
		parent->appendTask(this);
}

Task::~Task()
{
	//FIXME:some tasks are still in taskList but are destroyed here...
	//for (int i = 0; i < taskList.count(); i++)
	//	delete taskList[i];
	taskList.clear();
}

void Task::appendTask(Task* t)
{
	taskList << t;
}

void Task::processStanza(const Stanza& s)
{
//	printf("Task:: : %s\n", s.from().full().toLatin1().constData());
	processed = false;
//	printf("Count = %d\n", taskList.count());
	for (int i = 0; i < taskList.count(); i++)
	{
	//	printf("void Task::processStanza(const Stanza& s) : Kind = %s\n", s.kind() == Stanza::Presence ? "Presence" : "NoPresence");
		if (taskList[i]->canProcess(s))
		{
			//printf("[TASK] Ok, processing...\n");
			taskList[i]->processStanza(s);
			processed = true;
			break;
		}
		/*else
		{
			printf("[TASK] Won't process this stanza.\n");
		}*/
		/*printf("[TASK] Left : %d task(s)\n", taskList.count() - i);
		printf("[TASK] Next Task : ");
		Stanza fake(Stanza::BadStanza, "type-fake", "id-fake", "to-fake");
		i + 1 < taskList.count() ? taskList[i + 1]->canProcess(fake) : printf("NONE\n");*/
	}
	if (!processed)
	{
		printf("OO Must process that !\n");
	}
}

void Task::removeChild(Task* childTask)
{
	//printf("[TASK] Count = %d\n", taskList.count());
	for (int i = 0; i < taskList.count(); i++)
	{
		if (taskList[i] == childTask)
		{
//			printf("Ok, removing...\n");
			taskList.removeAt(i);
			break;
		}
	}

}

QString Task::randomString(int size)
{
	char *c;
	c = new char[size+1];
	for (int i = 0; i < size; i++)
	{
		int r;
		r = (rand()%26)+97;
		c[i] = r;
	}
	c[size] = '\0';
	return QString(c);
}
