#ifndef ROSTER_H
#define ROSTER_H

#include <QList>
#include <QString>

#include "jid.h"
#include "contact.h"

class Roster
{
public:
	Roster();
	~Roster();
	void addContact(const QString& jid, const QString& nickname, const QString& subscription);
	QList<Contact*> contactList() const;
	void clear();
private:
	QList<Contact*> c;
};
#endif
