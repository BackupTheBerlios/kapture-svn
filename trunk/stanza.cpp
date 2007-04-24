#include "stanza.h"

Stanza::Stanza()
{
	
}

Stanza::Stanza(QString kind, QString type, QString id)
{
	d = QDomDocument("");
	stan = d.createElement(kind);
	stan.setAttribute("type", type);
	if(!id.isEmpty())
		stan.setAttribute("id", id);
	d.appendChild(stan);
}

Stanza::Stanza(QByteArray n)
{
	// [....] ????
}

Stanza::~Stanza()
{

}

void Stanza::setType(QString s)
{
	stan.setTagName(s);
	/*type = BadStanza;
	badStanzaStr = s;
	if (s == "iq" || s == "IQ")
		type = IQ;
	if (s == "message" || s == "MESSAGE")
		type = Message;
	if (s == "presence" || s == "PRESENCE")
		type = Presence;
	printf(" * Received Stanza : %s.\n", getQStringType().toLatin1().constData());
	*/
}

QByteArray Stanza::getData()
{
	/*if (type == IQ)
		return "iq";
	if (type == Message)
		return "message";
	if (type == Presence)
		return "presence";
	return badStanzaStr;*/
	return d.toString().toLatin1();
}
