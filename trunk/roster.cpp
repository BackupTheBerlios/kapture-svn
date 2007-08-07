#include "roster.h"

Roster::Roster()
{

}

Roster::~Roster()
{

}

void Roster::addContact(const QString& jid, const QString& nickname, const QString& subscription)
{
/*
 * Subscription not managed yet
 */
	Contact *contact = new Contact(jid, nickname);
	c << contact;
}

QList<Contact*> Roster::contactList() const
{
	return c;
}
