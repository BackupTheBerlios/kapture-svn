#include "rosterModel.h"

Model::Model(QObject *parent)
	: QAbstractTableModel(parent)
{

}

Model::~Model()
{
	
}

void Model::setData(QStringList n)
{
	nodes = n;
}

QVariant Model::data(const QModelIndex &index, int role) const
{
	if (role == Qt::DecorationRole && index.column() == 2)
	{
	/*	if (files[index.row()].split(' ').count() == 4)
			return (QIcon("/home/detlev/src/extraits/yes.png"));
		else
			return (QIcon("/home/detlev/src/extraits/no.png"));
	*/}
	if (role == Qt::EditRole)
		return false;
	if (role == Qt::DisplayRole)
	{
		switch(index.column())
		{
			case 0: return nodes[index.row()];
		/*	case 1: return files[index.row()].split(' ')[1];
			case 2: return QVariant();*/
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
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch(section)
		{
			case 0: return tr("Contacts");
			/*case 1: return tr("Download date");
			case 2: return tr("Printed");*/
			default: return QVariant();
		}
	}
	if (orientation == Qt::Vertical && role == Qt::DisplayRole)
	{
		return section;
	}
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
	return 1;
}

