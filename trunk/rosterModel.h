#ifndef MODEL_H
#define MODEL_H
#include <QtCore>
#include <QtGui>
#include <QAbstractItemModel>

#include "jid.h"
#include "contact.h"

class Model : public QAbstractTableModel
{
	Q_OBJECT
public:
	Model(QObject *parent = 0);
	~Model();
	struct Nodes
	{
		Jid *jid;
		QString presenceType;
	};
	void setData(QList<Contact*>);
	void setData(QModelIndex index, QString value);
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QList<Contact*> getContactList();

private:
	QList<Contact*> contacts;
};
#endif
