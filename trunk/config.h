#ifndef CONFIG_H
#define CONFIG_H

#include "profile.h"

class Config
{
public:
	Config();
	~Config();
	QStringList getProfileNames();
	QList<Profile> getProfileList();
	QString getJid(QString profile);
	QString getPassword(QString profile);
	QString getPersonnalServer(QString profile);
	QString getPort(QString profile);
	bool noConfig;

private:
	QString cJid, cPassword, cPersonnalServer, cPort, cProfile;
	QList<Profile> profiles;
};



#endif //CONFIG_H
