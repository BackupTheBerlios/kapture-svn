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

#include <QString>

#include "jid.h"
#include "presence.h"

Presence::Presence(const Jid& from, const QString& type, const QString& status, const QString& show)
{
	j = from;
	t = type;
	stat = status;
	s = show;
	printf("j = %s, t = %s, stat = %s, s = %s\n",j.full().toLatin1().constData(), t.toLatin1().constData(), stat.toLatin1().constData(), s.toLatin1().constData());
}

Presence::~Presence()
{

}
