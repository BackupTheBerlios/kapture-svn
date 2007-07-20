#include "vcard.h"

VCard::VCard()
{
	nickname = "";
}

VCard::~VCard()
{

}

void VCard::setNickname(QString n)
{
	if (n == "")
		return;
	nickname = n;
}

QString VCard::getNickname()
{
	return nickname;
}
