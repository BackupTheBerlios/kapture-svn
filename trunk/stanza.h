#ifndef STANZA_H
#define STANZA_H

#include <QString>

class stanza
{
public:
	stanza();
	~stanza();
	void setType(QString s);
	QString getQStringType();
	enum stanzaType {IQ, Message, Presence, BadStanza} type;

private:
	QString badStanzaStr;

};

#endif //STANZA_H
