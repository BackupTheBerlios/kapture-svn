#ifndef PROFILE_H
#define PROFILE_H

#include <QString>

class Profile
{
public:
	Profile(QString name);
	~Profile();
	void setData(QString jid, QString password, QString server, QString port);
	QString getJid();
	QString getPassword();
	QString getPersonnalServer();
	QString getPort();
	QString getName();
	void setName(QString);
	void setJid(QString);
	void setPassword(QString);
	void setPersonnalServer(QString);
	void setPort(QString);
private:
	QString jid;
	QString password;
	QString server;
	QString port;
	QString profileName;
};

#endif
