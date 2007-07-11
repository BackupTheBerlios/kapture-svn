#ifndef JID_H
#define JID_H

#include <QString>
#include <QStringList>

class Jid
{
public:
	Jid(QString j);
	~Jid();
	QString toQString();
	bool isValid();
	QString getNode();
	void setNode(QString n);
	QString getResource();
	void setResource(QString);
	bool equals(Jid *other, bool withResource = false);
	bool operator==(Jid *other);
private:
	QString node;
	QString resource;
	bool valid;
};

#endif //JID_H
