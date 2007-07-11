#include "rosterModel.h"

Model::Model(QObject *parent)
	: QAbstractTableModel(parent)
{

}

Model::~Model()
{
	
}

void Model::setData(QList<Nodes> n)
{
	nodes = n;
}

void Model::setData(QModelIndex index, QString value)
{
	switch(index.column())
	{
		case 1:
			nodes[index.row()].jid->setNode(value);
			break;
		case 2:
			nodes[index.row()].presenceType = value;
			break;
	}
}

QVariant Model::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DecorationRole && index.column() == 0)
	{
		QImage *img;
		//printf(" ***** %s : %s *****\n", nodes[index.row()].node.toLatin1().constData(), nodes[index.row()].presenceType.toLatin1().constData());
		if (nodes[index.row()].presenceType == "unavailable")
			img = new QImage("offline.png");
		else
			img = new QImage("online.png");
			
		return *img;
	}
	if (role == Qt::EditRole)
		return false;
	if (role == Qt::DisplayRole)
	{
		switch(index.column())
		{
			case 1: return nodes[index.row()].jid->toQString();
			case 2: return nodes[index.row()].presenceType;
			default : return QVariant();
		}
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
	/*if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
			case 1: return tr("Contacts");
			case 2: return tr("State");
			default: return QVariant();
		}
	}
	if (orientation == Qt::Vertical && role == Qt::DisplayRole)
	{
		return section;
	}*/
	return QVariant();
}

QModelIndex Model::index(int row, int column, const QModelIndex &parent) const
{
	return createIndex(row, column, row);
}

int Model::rowCount(const QModelIndex &parent) const
{
	return nodes.count();
}

int Model::columnCount(const QModelIndex &parent) const
{
	// There will be more than 1 column
	return 3;
}

