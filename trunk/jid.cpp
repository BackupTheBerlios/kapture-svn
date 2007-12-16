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

#include "jid.h"


Jid::Jid()
{

}

Jid::Jid(const char*)
{

}

Jid::Jid(const QString& j)
{
	if (j.isEmpty())
	{
		valid = false;
		n = "";
		d = "";
		r = "";
		return;
	}

	valid = true;
        

	if(j.split('/').count() == 0)
	{
		printf("NOT VALID\n");
		valid = false;
        }
        
	if(j.split('/').count() == 1)
	{
		if (j.split('@').count() == 1)
		{
			n = "";
			r = "";
			d = j;
		}
		else
		{
			n = j.split('@').at(0);
			d = j.split('@').at(1);
		}
		// FIXME:Must manage errors
		//r = "";
		//n = "";
		//valid = false;
		//printf("n = %s, d = %s, r = %s\n", n.toLatin1().constData(), d.toLatin1().constData(), r.toLatin1().constData());
	}
	
	if(j.split('/').count() == 2)
	{
		n = j.split('@').at(0);
		d = j.split('@').at(1).split('/').at(0);
		r = j.split('/').at(1);
	}
}

Jid::~Jid()
{

}

QString Jid::full() const
{
	if (!valid)
		return QString();

	if (r != "")
	{
		return QString("%1@%2/%3").arg(n, d, r);
	}
	else
	{
		if (n != "")
			return QString("%1@%2").arg(n, d);
		else
			return d;

	}
}

bool Jid::isValid() const
{
	return valid;
}

QString Jid::node() const
{
	return n;
}

void Jid::setNode(const QString &node)
{
	n = node;
}

QString Jid::resource() const
{
	return r;
}

void Jid::setResource(const QString &resource)
{
	r = resource;
}

bool Jid::equals(const Jid& other, bool withResource)
{
	if (!valid || !other.isValid())
		return false;
	if (withResource)
	{
		if (n == other.node() && r == other.resource())
			return true;
		else
			return false;
	}
	else
	{
		if (n == other.node())
			return true;
		else
			return false;
	}
}

bool Jid::operator==(Jid &other) const
{
	if (!valid || !other.isValid())
		return false;
	
	if (r == "" || other.resource() == "")
	{
		if (n == other.node())
			return true;
		else
			return false;
	}
	else
	{
		if (n == other.node() && r == other.resource())
			return true;
		else
			return false;
		
	}
	return false;
}

QString Jid::domain() const
{
	return d;
}

QString Jid::bare() const // node@domain/resource --> bare = node@domain. Replace node !!!
{
	return n + '@' + d;
}
