#ifndef CONFIG_H
#define CONFIG_H
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QMessageBox>

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
	void addProfile(Profile);

private:
	QString cJid, cPassword, cPersonnalServer, cPort, cProfile;
	QList<Profile> profiles;
	QList<Profile> profiles2;
	QDomDocument d;
	QDomNodeList classes;
	int n;
};



#endif //CONFIG_H
