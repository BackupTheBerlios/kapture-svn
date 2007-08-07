#ifndef PRESENCE_H
#define PRESENCE_H

class Presence
{
public:
	Presence(const Jid& from, const QString& type, const QString& status, const QString& show);
	~Presence();
	Jid from() const {return j;};
	QString type() const {return t;};
	QString status() const {return stat;};
	QString show() const {return s;};
private:
	Jid j; // Jid
	QString t; // Type
	QString stat; // Status
	QString s; // Show
};

#endif //PRESENCE_H
