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

QString changeEmoticons(QString m)
{
	emoticons.append(Emoticon(":)", "smile.png"));
	emoticons.append(Emoticon(":-)", "smile.png"));
	// TODO: add more emoticons
	// FIXME:Find another way to have all emoticons here without refilling emoticons each times.
	for (int i = 0; i < emoticons.count(); i++)
	{
		m.replace(emoticons[i].binette, "<img src=\"" + emoticons[i].link + "\">");
	}
	return m;
}
