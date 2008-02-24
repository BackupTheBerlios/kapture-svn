#ifndef PROFILE_H
#define PROFILE_H

#include <QString>
#include "jid.h"

class Profile
{
public:
	Profile(QString &name);
	Profile();
	~Profile();
	void setData(Jid &jid, QString &password, QString &server, QString &port);
	Jid jid() const;
	QString password() const;
	QString personnalServer() const;
	QString port() const;
	QString name() const;
	void setName(QString&);
	void setJid(Jid&);
	void setPassword(QString&);
	void setPersonnalServer(QString&);
	void setPort(QString&);
private:
	Jid j;  // Jid
	QString p;  // Password
	QString s;  // Personnal server
	QString po; // Port
	QString n;  // Profile name 
};

#endif
