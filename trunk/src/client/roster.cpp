/*
 *      Kapture
 *
 *      Copyright (C) 2006-2009
 *          Detlev Casanova (detlev.casanova@gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

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
	Contact *contact = new Contact(jid, nickname, subscription);
	c << contact;
}

QList<Contact*> Roster::contactList() const
{
	return c;
}

void Roster::clear()
{
	c.clear();
}
