/*
 *      Kapture
 *
 *      Copyright (C) 2006-2007
 *          Detlev Casanova (detlev.casanova@gmail.com)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 */

#include "rosterModel.h"

Model::Model(QObject *parent)
	: QAbstractTableModel(parent)
{

}

Model::~Model()
{
	
}

void Model::setData(QList<Contact*> c)
{
	contacts = c;
}

void Model::setData(QModelIndex index, QString value)
{
	switch(index.column())
	{
		case 1:
			delete contacts[index.row()]->jid;
			contacts[index.row()]->jid = new Jid(value);
			break;
		/*case 2:
			contacts[index.row()].setPresence("", value);
			break;
		*/
	}
}

QVariant Model::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DecorationRole && index.column() == 0)
	{
		QImage *img;
		if (contacts[index.row()]->isAvailable())
			img = new QImage("online.png");
		else
			img = new QImage("offline.png");
			
		return *img;
	}
	if (role == Qt::EditRole)
		return false;
	if (role == Qt::DisplayRole)
	{
		switch(index.column())
		{
			case 1: return contacts[index.row()]->vCard()->nickname() == "" ? contacts[index.row()]->jid->full() : contacts[index.row()]->vCard()->nickname();
			//case 1 : return QString("Nickname or Jid");
			//case 2: return contacts[index.row()].getPresenceType();
			default : return QVariant();
		}
	}
	if (role == Qt::WhatsThisRole)
	{
		return contacts[index.row()]->jid->full();
	}
	return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant Model::headerData(int section, Qt::Orientation orientation, int role) const
{
	return QVariant();
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, row);
}

int Model::rowCount(const QModelIndex &parent) const
{
	return contacts.count();
}

int Model::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QList<Contact*> Model::getContactList()
{
	return contacts;
}
