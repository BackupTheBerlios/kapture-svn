#include "stanza.h"

stanza::stanza()
{

}

stanza::~stanza()
{

}

void stanza::setType(QString s)
{
	type = BadStanza;
	badStanzaStr = s;
	if (s == "iq" || s == "IQ")
		type = IQ;
	if (s == "message" || s == "MESSAGE")
		type = Message;
	if (s == "presence" || s == "PRESENCE")
		type = Presence;
	printf(" * Received Stanza : %s.\n", getQStringType().toLatin1().constData());
}

QString stanza::getQStringType()
{
	if (type == IQ)
		return "iq";
	if (type == Message)
		return "message";
	if (type == Presence)
		return "presence";
	return badStanzaStr; 
}
