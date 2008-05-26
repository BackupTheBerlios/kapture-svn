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
