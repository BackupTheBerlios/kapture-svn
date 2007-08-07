#include <QString>

#include "jid.h"
#include "presence.h"

Presence::Presence(const Jid& from, const QString& type, const QString& status, const QString& show)
{
	j = from;
	t = type;
	stat = status;
	s = show;
	printf("j = %s, t = %s, stat = %s, s = %s\n",j.full().toLatin1().constData(), t.toLatin1().constData(), stat.toLatin1().constData(), s.toLatin1().constData());
}

Presence::~Presence()
{

}
