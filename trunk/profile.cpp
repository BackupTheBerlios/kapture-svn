#include "profile.h"

Profile::Profile(QString& name)
{
	n = name;
}

Profile::Profile()
{

}

Profile::~Profile()
{

}

void Profile::setData(QString& jid, QString& pass, QString& server, QString& port)
{
	j = jid;
	p = pass;
	s = server;
	po = port;
}
	
QString Profile::jid() const
{
	return j;
}

QString Profile::password() const
{
	return p;
}

QString Profile::personnalServer() const
{
	return s;
}

QString Profile::port() const
{
	return po;
}

QString Profile::name() const
{
	return n;
}

void Profile::setName(QString& name)
{
	n = name;
}

void Profile::setJid(QString& jid)
{
	j = jid;
}

void Profile::setPassword(QString& pass)
{
	p = pass;
}

void Profile::setPersonnalServer(QString& server)
{
	s = server;
}

void Profile::setPort(QString& port)
{
	po = port;
}

