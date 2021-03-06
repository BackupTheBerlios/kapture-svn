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

#include "profilemodel.h"

#include <QString>

ProfileModel::ProfileModel(QObject*)
{

}

ProfileModel::~ProfileModel()
{

}

void ProfileModel::setProfileList(QList<Profile> p)
{
	profiles = p;
}

void ProfileModel::setData(QModelIndex index, QString value)
{
	Jid jid;
	switch(index.column())
	{
		case 0:
			profiles[index.row()].setName(value);
			break;
		case 1:
			jid = Jid(value);
			profiles[index.row()].setJid(jid);
			break;
		case 2:
			profiles[index.row()].setPersonnalServer(value);
			break;
		case 3:
			profiles[index.row()].setPort(value);
			break;
	}
}


QVariant ProfileModel::data(const QModelIndex &index, int role) const
{
	QString a;
	Profile tmp = profiles[index.row()];

	if (role == Qt::DisplayRole)
	{
		switch(index.column())
		{
			case 0: return tmp.name();
			case 1: return tmp.jid().full();
			case 2: return tmp.personnalServer();
			case 3: return tmp.port();
			//case 4: return profiles[index.row()].getPort();
			// FIXME:Why doesn't that work ?????
			default : return QVariant();
		}
	}
	return QVariant();
}

Qt::ItemFlags ProfileModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant ProfileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
			case 0: return tr("Profile");
			case 1: return tr("Jid");
			case 2: return tr("Server");
			case 3: return tr("Port");
			default: return QVariant();
		}
	}
	if (orientation == Qt::Vertical && role == Qt::DisplayRole)
	{
		return section + 1;
	}
	return QVariant();
}

QModelIndex ProfileModel::index(int row, int column, const QModelIndex&) const
{
	return createIndex(row, column, row);
}

int ProfileModel::rowCount(const QModelIndex&) const
{
	return profiles.count();
}

int ProfileModel::columnCount(const QModelIndex&) const
{
	return 4;
}

bool ProfileModel::insertRow(int position, const QModelIndex&)
{
	beginInsertRows(QModelIndex(), position, position);

	QString test = "";
	Profile p(test);
	profiles.insert(position, p);

	endInsertRows();
	return true;
}

bool ProfileModel::removeRow(int position, const QModelIndex&)
{
	beginRemoveRows(QModelIndex(), position, position);

	profiles.removeAt(position);

	endRemoveRows();
	return true;
}
