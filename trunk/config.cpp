#include "config.h"

Config::Config()
{
	int i = 0;
	bool found = false;
	noConfig = false;
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
		noConfig = true;
		QMessageBox::critical(0, QString("Jabber"), QString("Unable to create config file. Check permissions for\n \"$HOME/.Kapture/conf.xml\""), QMessageBox::Ok);
		return;
	}
	
	QFile *conf = new QFile(QDir::homePath() + "/.Kapture/conf.xml");
	conf->open(QIODevice::ReadOnly);
	config = conf->readAll();
	d.setContent(config);

	if (d.documentElement().tagName() != "Kapture")
	{
		noConfig = true;
		return;
	}

	classes = d.documentElement().childNodes();
	for(i = 0; i < classes.count() ;i++)
	{
		if (classes.at(i).toElement().tagName() == "xmppwin")
		{
			found = true;
			n = i;
			break;
		}
	}
	if (!found)
	{
		noConfig = true;
		return;
	}
	
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
		profiles2 << *profile;
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
	return profiles2;
}

QString Config::getJid(QString profile)
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profile)
			return profiles[i].getJid();
	}
	return "";
}

QString Config::getPassword(QString profile)
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profile)
			return profiles[i].getPassword();
	}
	return "";
}

QString Config::getPersonnalServer(QString profile)
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profile)
			return profiles[i].getPersonnalServer();
	}
	return "";
}

QString Config::getPort(QString profile)
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].getName() == profile)
			return profiles[i].getPort();
	}
	return "";
}

void Config::addProfile(Profile p)
{
	if (noConfig)
	{
		// Create XML tree from scratch.
		QDomDocument *doc = new QDomDocument();
		QDomElement kap = doc->createElement("Kapture");
		kap.setAttribute("xmlns", "http://kapture.berlios.de/config");
		QDomElement classe = doc->createElement("xmppwin");
		QDomElement prof = doc->createElement("profile");
		prof.setAttribute("name", p.getName());
		
		QDomElement eJid = doc->createElement("jid");
		QDomText vJid = doc->createTextNode(p.getJid());
		eJid.appendChild(vJid);
		
		QDomElement ePassword = doc->createElement("password");
		QDomText vPassword = doc->createTextNode(p.getPassword());
		ePassword.appendChild(vPassword);
		
		QDomElement eServer = doc->createElement("server");
		QDomText vServer = doc->createTextNode(p.getPersonnalServer());
		eServer.appendChild(vServer);
		
		QDomElement ePort = doc->createElement("port");
		QDomText vPort = doc->createTextNode(p.getPort());
		ePort.appendChild(vPort);

		prof.appendChild(eJid);
		prof.appendChild(ePassword);
		prof.appendChild(eServer);
		prof.appendChild(ePort);
		classe.appendChild(prof);
		kap.appendChild(classe);
		doc->appendChild(kap);
		
		QFile *file = new QFile(QDir::homePath() + "/.Kapture/conf.xml");
		file->open(QIODevice::WriteOnly);
		if (!file->exists())
		{
			printf("An error occured while writing the file.\n");
			return; // An error occured (QMessageBox)
		}
		file->write(doc->toByteArray(1));
		delete file;
	}
	else
	{
		QDomElement prof = d.createElement("profile");
		prof.setAttribute("name", p.getName());
		
		QDomElement eJid = d.createElement("jid");
		QDomText vJid = d.createTextNode(p.getJid());
		eJid.appendChild(vJid);
		
		QDomElement ePassword = d.createElement("password");
		QDomText vPassword = d.createTextNode(p.getPassword());
		ePassword.appendChild(vPassword);
		
		QDomElement eServer = d.createElement("server");
		QDomText vServer = d.createTextNode(p.getPersonnalServer());
		eServer.appendChild(vServer);
		
		QDomElement ePort = d.createElement("port");
		QDomText vPort = d.createTextNode(p.getPort());
		ePort.appendChild(vPort);

		prof.appendChild(eJid);
		prof.appendChild(ePassword);
		prof.appendChild(eServer);
		prof.appendChild(ePort);
		classes.at(n).appendChild(prof);
		
		QFile *file = new QFile(QDir::homePath() + "/.Kapture/conf.xml");
		file->open(QIODevice::WriteOnly);
		if (!file->exists())
		{
			printf("An error occured while writing the file.\n");
			return; // An error occured (QMessageBox)
		}
		file->write(d.toByteArray(1));
		delete file;
	}

}
	
void Config::delProfile(QString profileName)
{
	// If user deletes a profile, it means that the configuration is valid.
	QDomNodeList p = classes.at(n).childNodes();
	for (int i = 0; i < p.count(); i++)
	{
		if (p.at(i).toElement().attribute("name") == profileName)
		{
			classes.at(n).removeChild(p.at(i));
			break;
		}
	}
	
	QFile *file = new QFile(QDir::homePath() + "/.Kapture/conf.xml");
	file->open(QIODevice::WriteOnly);
	if (!file->exists())
	{
		printf("An error occured while writing the file.\n");
		return; // An error occured (QMessageBox)
	}
	file->write(d.toByteArray(1));
	delete file;
}
