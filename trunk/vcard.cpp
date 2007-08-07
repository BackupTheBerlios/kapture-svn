#include "vcard.h"

VCard::VCard()
{
	n = "";
}

VCard::~VCard()
{

}

void VCard::setNickname(const QString& nickname)
{
	if (nickname == "")
		n = "";
	else
		n = nickname;
//	printf("VCard::setNickname : Nickname set\n");
}

QString VCard::nickname() const
{
	return n;
}
