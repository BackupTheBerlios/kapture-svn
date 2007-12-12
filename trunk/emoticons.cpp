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

#include "emoticons.h"

Emoticons::Emoticons()
{
	emoticons.append(Emoticon(":)", "smile.png"));
	emoticons.append(Emoticon(":-)", "smile.png"));
	// TODO: add more emoticons and load other from file or directory.
}

QString Emoticons::changeEmoticons(const QString& m/*, QString jid*/)
{
	QString temp = m;
	for (int i = 0; i < emoticons.count(); i++)
	{
		temp.replace(emoticons[i].binette, "<img src=\"" + QString(DATADIR) + QString("/emoticons/") + emoticons[i].link + "\">");
	}
	//if (m.startsWith("/me "))
	//	m = "<font color='green'> *** " + m.split("/me") + "</font>";
	// FIXME:*message* works a half
	if (temp.at(0) == '*' && temp.endsWith('*') && !temp.contains(" "))
		temp = "<b>" + temp + "</b>";
	return temp;
}
