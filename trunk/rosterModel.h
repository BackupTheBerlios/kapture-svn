#ifndef MODEL_H
#define MODEL_H
#include <QtCore>
#include <QtGui>
#include <QAbstractItemModel>

class Model : public QAbstractTableModel
{
	Q_OBJECT
public:
	Model(QObject *parent = 0);
	~Model();
	struct Nodes
	{
		QString node;
		QString state;
	};
	void setData(QList<Nodes> n);
	void setData(QModelIndex index, QString value);
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;

private:
	QList<Nodes> nodes;
};
#endif
