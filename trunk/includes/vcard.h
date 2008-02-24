#ifndef VCARD_H
#define VCARD_H

#include <QString>

class VCard
{
public:
	VCard();
	~VCard();
	void setNickname(const QString &n);
	QString nickname() const;
private:
	QString n; // Nickname
};

#endif
