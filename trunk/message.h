#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include "jid.h"

class Message
{
public:
	Message(const Jid& from,
		const Jid& to,
		const QString& message,
		const QString& type,
		const QString& subject,
		const QString& thread);
	~Message();
	Jid from() const {return f;};
	Jid to() const {return t;};
	QString message() const {return m;};
	QString type() const {return ty;};
private:
	Jid f; // From
	Jid t; // To
	QString m; // Message
	QString ty; // Type
	QString s; // Subject
	QString tr; // Thread
};
#endif
