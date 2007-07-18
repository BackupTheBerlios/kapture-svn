#include <QFile>
#include <QDir>
#include <QDomDocument>

#include "config.h"

Config::Config()
{
	int i = 0;
	bool found = false;
	QByteArray config;
	//Loads last configuration from ~/.Kapture/conf.xml
	QDir *confDir = new QDir(QDir::homePath() + "/.Kapture/");
	if (!confDir->exists())
	{
		QDir::home().mkdir(".Kapture");
	}
	confDir->setPath(QDir::homePath() + "/.Kapture/");
	
	if (!confDir->exists())
	{
	//	noConfig = true;
		return;
	}
	
	QFile *conf = new QFile(QDir::homePath() + "/.Kapture/conf.xml");
	conf->open(QIODevice::ReadOnly);
	config = conf->readAll();
	printf("Config = %s\n", config.constData());
	QDomDocument d;
	d.setContent(config);

	
	if (d.documentElement().tagName() != "Kapture")
		return; // TODO:Should say it !

	QDomNodeList classes = d.documentElement().childNodes();
	for(i = 0; i < classes.count() ;i++)
	{
		if (classes.at(i).toElement().tagName() == "xmppwin")
		{
			found = true;
			break;
		}
	}
	if (!found)
		return;
	
	QDomNodeList profilesNodeList = classes.at(i).childNodes();
	for (int j = 0; j < profilesNodeList.count(); j++)
	{
		cJid = "";
		cPassword = "";
		cPersonnalServer = "";
		cPort = "5222";
		cProfile = profilesNodeList.at(j).toElement().attribute("name");
		QDomNodeList infos = profilesNodeList.at(j).childNodes();
		for (i = 0; i < infos.count(); i++)
		{
			if (infos.at(i).toElement().tagName() == "jid" && infos.at(i).hasChildNodes())
				cJid = infos.at(i).firstChild().toText().data();

			if (infos.at(i).toElement().tagName() == "password" && infos.at(i).hasChildNodes())
				cPassword = infos.at(i).firstChild().toText().data();

			if (infos.at(i).toElement().tagName() == "server" && infos.at(i).hasChildNodes())
				cPersonnalServer = infos.at(i).firstChild().toText().data();

			if (infos.at(i).toElement().tagName() == "port" && infos.at(i).hasChildNodes())
				cPort = infos.at(i).firstChild().toText().data();
		}
		Profile *profile = new Profile(cProfile);
		profile->setData(cJid, cPassword, cPersonnalServer, cPort);
		profiles << *profile;
	}
}

Config::~Config()
{

}

QStringList Config::getProfileNames()
{
	QStringList p;
	for (int i = 0; i < profiles.count(); i++)
	{
		p << profiles[i].getName();
	}
	return p;
}

QList<Profile> Config::getProfileList()
{
	return profiles;
}

QString Config::getJid(QString profile)
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profile)
			return profiles[i].getJid();
	}
}

QString Config::getPassword(QString profile)
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profile)
			return profiles[i].getPassword();
	}
}

QString Config::getPersonnalServer(QString profile)
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profile)
			return profiles[i].getPersonnalServer();
	}
}

QString Config::getPort(QString profile)
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profile)
			return profiles[i].getPort();
	}
}

