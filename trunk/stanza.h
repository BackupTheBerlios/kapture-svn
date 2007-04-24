#ifndef STANZA_H
#define STANZA_H

#include <QString>
#include <QtXml>

class Stanza
{
public:
	enum Kind {IQ, Message, Presence, BadStanza};
	Stanza();
	Stanza(QByteArray node);
	Stanza(QString kind, QString type, QString id);
	~Stanza();
	void setType(QString s);
	//QString getQStringType(Kind kind);
	QByteArray getData();

private:
	QString badStanzaStr;
	QDomDocument d;
	QDomElement stan;

};

#endif //STANZA_H
