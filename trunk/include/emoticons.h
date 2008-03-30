#ifndef UTILS_H
#define UTILS_H
#include <QString>
#include <QList>

class Emoticons
{
public:
	Emoticons();
	QString changeEmoticons(const QString&);
private:
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
	
	~Emoticons();
};

#endif
