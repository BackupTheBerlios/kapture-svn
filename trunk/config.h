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
	QStringList profileNames() const;
	QList<Profile> profileList() const;
	QString jid(QString &profile) const;
	QString password(QString &profile) const;
	QString personnalServer(QString &profile) const;
	QString port(QString &profile) const;
	bool noConfig;
	void addProfile(const Profile &profile);
	void delProfile(const QString &profileName);

private:
	QString cJid, cPassword, cPersonnalServer, cPort, cProfile;
	QList<Profile> profiles;
	QDomDocument d;
	QDomNodeList classes;
	int n; // another name... (xmppwin class' position in XML tree -- this class system will be removed)
};



#endif //CONFIG_H
