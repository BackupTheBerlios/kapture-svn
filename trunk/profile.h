#ifndef PROFILE_H
#define PROFILE_H

#include <QString>

class Profile
{
public:
	Profile(QString &name);
	Profile();
	~Profile();
	void setData(QString &jid, QString &password, QString &server, QString &port);
	QString jid() const;
	QString password() const;
	QString personnalServer() const;
	QString port() const;
	QString name() const;
	void setName(QString&);
	void setJid(QString&);
	void setPassword(QString&);
	void setPersonnalServer(QString&);
	void setPort(QString&);
private:
	/*Jid*/
	QString j;  // Jid
	/*Jid*/
	QString p;  // Password
	QString s;  // Personnal server
	QString po; // Port
	QString n;  // Profile name 
};

#endif
