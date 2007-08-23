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

#include "utils.h"

struct Emoticon
{
	QString binette; // :-)
	QString link; // smile.png
	Emoticon(QString b, QString l)
	{
		binette = b;
		link = l;
	}
};
QList<Emoticon> emoticons;

QString changeEmoticons(QString m/*, QString jid*/)
{
	emoticons.append(Emoticon(":)", "smile.png"));
	emoticons.append(Emoticon(":-)", "smile.png"));
	// TODO: add more emoticons
	// FIXME:Find another way to have all emoticons here without refilling emoticons each times.
	for (int i = 0; i < emoticons.count(); i++)
	{
		m.replace(emoticons[i].binette, "<img src=\"" + emoticons[i].link + "\">");
	}
	//if (m.startsWith("/me "))
	//	m = "<font color='green'> *** " + m.split("/me") + "</font>";
	// FIXME:*message* works a half
	if (m.at(0) == '*' && m.endsWith('*') && !m.contains(" "))
		m = "<b>" + m + "</b>";
	return m;
}
