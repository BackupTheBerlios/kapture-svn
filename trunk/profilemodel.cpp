#include "profilemodel.h"

#include <QString>

ProfileModel::ProfileModel(QObject *parent)
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
	switch(index.column())
	{
		case 0:
			profiles[index.row()].setName(value);
			break;
		case 1:
			profiles[index.row()].setJid(value);
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
	int b = index.row();
	/*if (role == Qt::DecorationRole && index.column() == 0)
	{
		QImage *img;
		if (nodes[index.row()].presenceType == "unavailable")
			img = new QImage("offline.png");
		else
			img = new QImage("online.png");
			
		return *img;
	}
	if (role == Qt::EditRole)
		return false;
	*/

	Profile tmp = profiles[index.row()];

	if (role == Qt::DisplayRole)
	{
		switch(index.column())
		{
			case 0: return tmp.getName();
			case 1: return tmp.getJid();
			case 2: return tmp.getPersonnalServer();
			case 3: return tmp.getPort();
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
		return section;
	}
	return QVariant();
}

QModelIndex ProfileModel::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, row);
}

int ProfileModel::rowCount(const QModelIndex &parent) const
{
	return profiles.count();
}

int ProfileModel::columnCount(const QModelIndex &parent) const
{
	return 4;
}
