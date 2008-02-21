#ifndef CONFIG_H
#define CONFIG_H
#include <QFile>
#include <QDir>
#include <QDomDocument>
#include <QMessageBox>

#include "profile.h"
#include "jid.h"

class Config
{
public:
	Config();
	~Config();
	QStringList profileNames() const;
	QList<Profile> profileList() const;
	Jid jid(QString &profile) const;
	QString password(QString &profile) const;
	QString personnalServer(QString &profile) const;
	QString port(QString &profile) const;
	bool noConfig;
	void addProfile(const Profile &profile);
	void delProfile(const QString &profileName);
	bool useSystemTray() const;
	
	int ftPort() const;
	QString proxy() const;
	QString resource() const;
	void setFTPort(int);
	void setProxy(const QString&);
	void setResource(const QString&);

private:
	QString cPassword, cPersonnalServer, cPort, cProfile,
		cFTPort, cProxy, cResource;
	Jid cJid;
	bool cUseSysTray;
	QList<Profile> profiles;
	QDomDocument d;
	QDomNodeList classes;
	int xmppwinN;
	int connectionN;
	int otherN;
	bool xmppwinFound;
	bool connectionFound;
	bool otherFound;
};



#endif //CONFIG_H
