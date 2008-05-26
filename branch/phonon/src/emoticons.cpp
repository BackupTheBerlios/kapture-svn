/*
 *      Kapture -- emoticons.cpp
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

#include <QtCore>
#include <QStringList>

#include "emoticons.h"

Emoticons::Emoticons()
{
	emoticons.append(Emoticon(":)", "smile.png"));
	emoticons.append(Emoticon(":-)", "smile.png"));

	// TODO: add more emoticons and load other from file or directory.
}

QString Emoticons::changeEmoticons(const QString& m/*, QString jid*/)
{
	//QString temp = Qt::escape(m); --> FIXME:Why isn't escape a member of Qt (/usr/share/doc/qt-4.3.3/html/qt.html#escape)
	QString temp = m;
	for (int i = 0; i < emoticons.count(); i++)
	{
		temp.replace(emoticons[i].binette, "<img src=\"" + QString(DATADIR) + QString("/emoticons/") + emoticons[i].link + "\">");
	}
	//if (m.startsWith("/me "))
	//	temp = QString("<font color='green'><i> *** ") + temp.split("/me").at(0) + QString("</i></font>");
	//else
	//	temp = QString("<font color='red'>%1 says :</font><br>") + temp + QString("<br>");
	// FIXME:*message* works a half
	if (temp.at(0) == '*' && temp.endsWith('*') && !temp.contains(" "))
		temp = "<b>" + temp + "</b>";
	return temp;
}
