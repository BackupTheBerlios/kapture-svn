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

#include <QString>

#include "message.h"
#include "jid.h"

Message::Message(const Jid& from,
		 const Jid& to,
		 const QString& message,
		 const QString& type,
		 const QString& subject,
		 const QString& thread)
{
	f = from;
	t = to;
	m = message;
	ty = type;
	s = subject;
	tr = thread;
}

Message::~Message()
{}

