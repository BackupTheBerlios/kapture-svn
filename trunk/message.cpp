#include <QString>

#include "message.h"
#include "jid.h"

Message::Message(const Jid& from,
		 const Jid& to,
		 const QString& message,
		 const QString& type,
		 const QString& subject,
		 const QString& thread)
{
	f = from;
	t = to;
	m = message;
	ty = type;
	s = subject;
	tr = thread;
}

Message::~Message()
{}

