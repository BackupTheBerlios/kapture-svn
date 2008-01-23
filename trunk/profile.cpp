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

#include "profile.h"

Profile::Profile(QString& name)
{
	n = name;
}

Profile::Profile()
{

}

Profile::~Profile()
{

}

void Profile::setData(QString& jid, QString& pass, QString& server, QString& port)
{
	j = jid;
	p = pass;
	s = server;
	po = port;
}
	
QString Profile::jid() const
{
	return j;
}

Jid Profile::jid1() const
{
	return Jid(j);
}

QString Profile::password() const
{
	return p;
}

QString Profile::personnalServer() const
{
	return s;
}

QString Profile::port() const
{
	return po;
}

QString Profile::name() const
{
	return n;
}

void Profile::setName(QString& name)
{
	n = name;
}

void Profile::setJid(QString& jid)
{
	j = jid;
}

void Profile::setPassword(QString& pass)
{
	p = pass;
}

void Profile::setPersonnalServer(QString& server)
{
	s = server;
}

void Profile::setPort(QString& port)
{
	po = port;
}

