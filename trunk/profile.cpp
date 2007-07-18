#include "profile.h"

Profile::Profile(QString n)
{
	profileName = n;
}

Profile::~Profile()
{

}

void Profile::setData(QString j, QString pw, QString s, QString p)
{
	jid = j;
	password = pw;
	server = s;
	port = p;
}
	
QString Profile::getJid()
{
	return jid;
}

QString Profile::getPassword()
{
	return password;
}

QString Profile::getPersonnalServer()
{
	return server;
}

QString Profile::getPort()
{
	return port;
}

QString Profile::getName()
{
	return profileName;
}

void Profile::setName(QString n)
{
	profileName = n;
}

void Profile::setJid(QString j)
{
	jid = j;
}

void Profile::setPassword(QString p)
{
	password = p;
}

void Profile::setPersonnalServer(QString p)
{
	server = p;
}

void Profile::setPort(QString p)
{
	port = p;
}

