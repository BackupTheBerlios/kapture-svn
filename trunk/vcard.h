#ifndef VCARD_H
#define VCARD_H

#include <QString>

class VCard
{
public:
	VCard();
	~VCard();
	void setNickname(QString n);
	QString getNickname();
private:
	QString nickname;
};

#endif
