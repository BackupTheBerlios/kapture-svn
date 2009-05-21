/*
 *      Kapture
 *
 *      Copyright (C) 2006-2009
 *          Detlev Casanova (detlev.casanova@gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

#include "config.h"

Config::Config()
{
	int i = 0;
	xmppwinFound = false;
	connectionFound = false;
	otherFound = false;
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
			xmppwinFound = true;
			xmppwinN = i;
		}

		if (classes.at(i).toElement().tagName() == "connection")
		{
			connectionFound = true;
			connectionN = i;
		}

		if (classes.at(i).toElement().tagName() == "other")
		{
			otherFound = true;
			otherN = i;
		}
	}

	// Loading xmppwin class data.
	if (xmppwinFound)
	{
		QDomNodeList profilesNodeList = classes.at(xmppwinN).childNodes();
		for (int j = 0; j < profilesNodeList.count(); j++)
		{
			cJid = Jid("");
			cPassword = "";
			cPersonnalServer = "";
			cPort = "5222";
			cProfile = profilesNodeList.at(j).toElement().attribute("name");
			QDomNodeList infos = profilesNodeList.at(j).childNodes();
			for (i = 0; i < infos.count(); i++)
			{
				if (infos.at(i).toElement().tagName() == "jid" && infos.at(i).hasChildNodes())
					cJid = Jid(infos.at(i).firstChild().toText().data());
	
				if (infos.at(i).toElement().tagName() == "password" && infos.at(i).hasChildNodes())
					cPassword = QByteArray::fromBase64(infos.at(i).firstChild().toText().data().toLatin1());
	
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

	// Loading connection class data
	if (connectionFound)
	{
		QDomNodeList connectionNodeList = classes.at(connectionN).childNodes();
		for (int i = 0; i < connectionNodeList.count(); i++)
		{
			// File Transfer Port
			if (connectionNodeList.at(i).localName() == "ftport")
				cFTPort = connectionNodeList.at(i).toElement().attribute("val", "8010");
			// Proxy
			if (connectionNodeList.at(i).localName() == "proxy")
				cProxy = connectionNodeList.at(i).toElement().attribute("val", "");
			// Resource
			if (connectionNodeList.at(i).localName() == "resource")
				cResource = connectionNodeList.at(i).toElement().attribute("val", "Kapture");
		}
	}
	else
	{
		// Default Values
		cFTPort = "8010";
		cProxy = "";
		cResource = "Kapture";
	}

	// Loading Other class data
	if (otherFound)
	{
		QDomNodeList otherNodeList = classes.at(otherN).childNodes();
		for (int i = 0; i < otherNodeList.count(); i++)
		{
			if (otherNodeList.at(i).localName() == "systray")
				cUseSysTray = otherNodeList.at(i).toElement().attribute("val", "true") == "true" ? true : false;
		}
	}
	else
	{
		// Default Values.
		cUseSysTray = true;
	}
	/* TODO:
	 * 	Should create an initial config if there's none at all.
	 */
}

Config::~Config()
{

}

bool Config::useSystemTray() const
{
	return cUseSysTray;
}

QStringList Config::profileNames() const
{
	QStringList p;
	for (int i = 0; i < profiles.count(); i++)
	{
		p << profiles[i].name();
	}
	return p;
}

QList<Profile> Config::profileList() const
{
	return profiles;
}

Jid Config::jid(QString& profile) const
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].name() == profile)
			return profiles[i].jid();
	}
	return "";
}

QString Config::password(QString& profile) const
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].name() == profile)
			return profiles[i].password();
	}
	return "";
}

QString Config::personnalServer(QString& profile) const
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].name() == profile)
			return profiles[i].personnalServer();
	}
	return "";
}

QString Config::port(QString& profile) const
{
	for (int i = 0; i < profiles.count(); i++)
	{
		if (profiles[i].name() == profile)
			return profiles[i].port();
	}
	return "";
}

void Config::addProfile(const Profile& p)
{
	if (noConfig)
	{
		// Create XML tree from scratch.
		QDomDocument *doc = new QDomDocument();
		QDomElement kap = doc->createElement("Kapture");
		kap.setAttribute("xmlns", "http://kapture.berlios.de/config");
		QDomElement classe = doc->createElement("xmppwin");
		QDomElement prof = doc->createElement("profile");
		prof.setAttribute("name", p.name());
		
		QDomElement eJid = doc->createElement("jid");
		QDomText vJid = doc->createTextNode(p.jid().full());
		eJid.appendChild(vJid);
		
		QDomElement ePassword = doc->createElement("password");
		QDomText vPassword = doc->createTextNode(p.password().toLatin1().toBase64());
		ePassword.appendChild(vPassword);
		
		QDomElement eServer = doc->createElement("server");
		QDomText vServer = doc->createTextNode(p.personnalServer());
		eServer.appendChild(vServer);
		
		QDomElement ePort = doc->createElement("port");
		QDomText vPort = doc->createTextNode(p.port());
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
		if (!xmppwinFound)
		{
			QDomElement classe = d.createElement("xmppwin");
			//classes << classe; FIXME:there should be a way to do that (QList<QDomNode>)
			xmppwinN = classes.count() - 1;
			d.documentElement().appendChild(classe);
			xmppwinFound = true;
		}
		QDomElement prof = d.createElement("profile");
		prof.setAttribute("name", p.name());
		
		QDomElement eJid = d.createElement("jid");
		QDomText vJid = d.createTextNode(p.jid().full());
		eJid.appendChild(vJid);
		
		QDomElement ePassword = d.createElement("password");
		QDomText vPassword = d.createTextNode(p.password().toLatin1().toBase64());
		ePassword.appendChild(vPassword);
		
		QDomElement eServer = d.createElement("server");
		QDomText vServer = d.createTextNode(p.personnalServer());
		eServer.appendChild(vServer);
		
		QDomElement ePort = d.createElement("port");
		QDomText vPort = d.createTextNode(p.port());
		ePort.appendChild(vPort);

		prof.appendChild(eJid);
		prof.appendChild(ePassword);
		prof.appendChild(eServer);
		prof.appendChild(ePort);
		classes.at(xmppwinN).appendChild(prof);
		
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
	
void Config::delProfile(const QString &profileName)
{
	// If user deletes a profile, it means that the configuration is valid.
	QDomNodeList p = classes.at(xmppwinN).childNodes();
	for (int i = 0; i < p.count(); i++)
	{
		if (p.at(i).toElement().attribute("name") == profileName)
		{
			classes.at(xmppwinN).removeChild(p.at(i));
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

void Config::setFTPort(int port)
{
	// Not Implemented YET !!!!!!!!!!!!!!!!!!!!!!
	cFTPort = port;
}

void Config::setProxy(const QString& proxy)
{
	// Not Implemented YET !!!!!!!!!!!!!!!!!!!!!!
	cProxy = proxy;
}

void Config::setResource(const QString& resource)
{
	// Not Implemented YET !!!!!!!!!!!!!!!!!!!!!!
	cResource = resource;
}
	
int Config::ftPort() const
{
	return cFTPort.toInt();
}

QString Config::proxy() const
{
	return cProxy;
}

QString Config::resource() const
{
	return cResource;
}
