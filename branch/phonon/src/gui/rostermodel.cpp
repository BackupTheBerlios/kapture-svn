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

#include "rostermodel.h"

Model::Model(QObject *parent)
	: QAbstractTableModel(parent)
{

}

Model::~Model()
{
	
}

void Model::setData(QList<Contact*> *c)
{
	contacts = c;
}

void Model::setData(QModelIndex index, QString value)
{
	switch(index.column())
	{
		case 1:
			delete contacts->at(index.row())->jid;
			contacts->at(index.row())->jid = new Jid(value);
			break;
		/*case 2:
			contacts[index.row()].setPresence("", value);
			break;
		*/
	}
}

QString showToPretty(const QString& show)
{
	if (show.toLower() == "dnd")
		return QString("busy");
	if (show.toLower() == "chat")
		return QString("available to chat");
	if (show.toLower() == "away")
		return QString("away");
	if (show.toLower() == "xa")
		return QString("far far away");
	return QString();
}

QVariant Model::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DecorationRole && index.column() == 0)
	{
		QImage *img;
		if (contacts->at(index.row())->isAvailable())
		{
			//FIXME: 'img' may be used uninitialized in this function. (Says gcc...)
			//printf("[RosterModel] Contact is %s\n", contacts->at(index.row())->show().toLatin1().constData());
			if (contacts->at(index.row())->show() == "")
			{
				if (contacts->at(index.row())->subscription() == "both")
					img = new QImage(QString(DATADIR) + QString("/icons/") + "online.png");
				if (contacts->at(index.row())->subscription() == "from")
					img = new QImage(QString(DATADIR) + QString("/icons/") + "online-from.png");
				if (contacts->at(index.row())->subscription() == "to")
					img = new QImage(QString(DATADIR) + QString("/icons/") + "online-to.png");
				if (contacts->at(index.row())->subscription() == "none")
					img = new QImage(QString(DATADIR) + QString("/icons/") + "online-none.png");
			}
			if (contacts->at(index.row())->show() == "away")
				img = new QImage(QString(DATADIR) + QString("/icons/") + "away.png");
			if (contacts->at(index.row())->show() == "chat")
				img = new QImage(QString(DATADIR) + QString("/icons/") + "chat.png");
			if (contacts->at(index.row())->show() == "xa")
				img = new QImage(QString(DATADIR) + QString("/icons/") + "xa.png");
			if (contacts->at(index.row())->show() == "dnd")
				img = new QImage(QString(DATADIR) + QString("/icons/") + "dnd.png");

		}
		else
			img = new QImage(QString(DATADIR) + QString("/icons/") + "offline.png");
			
		return *img;
	}
	if (role == Qt::EditRole)
		return false;
	if (role == Qt::DisplayRole)
	{
		QString str;
		switch(index.column())
		{
			case 1: 
				str = contacts->at(index.row())->vCard()->nickname() == "" ? contacts->at(index.row())->jid->full() : contacts->at(index.row())->vCard()->nickname();
				if (contacts->at(index.row())->isAvailable() && contacts->at(index.row())->show() != "")
					str = str + QString(" (") + showToPretty(contacts->at(index.row())->show()) + QString(")");
				return str;
				break;
			default : return QVariant();
		}
	}
	if (role == Qt::WhatsThisRole)
	{
		return contacts->at(index.row())->jid->full();
	}
	return QVariant();
}

Qt::ItemFlags Model::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return Qt::ItemIsEnabled;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant Model::headerData(int, Qt::Orientation, int) const
{
	return QVariant();
}

QModelIndex Model::index(int row, int column, const QModelIndex&) const
{
	return createIndex(row, column, row);
}

int Model::rowCount(const QModelIndex&) const
{
	return contacts->count();
}

int Model::columnCount(const QModelIndex&) const
{
	return 2;
}

QList<Contact*> Model::getContactList()
{
	return *contacts;
}
