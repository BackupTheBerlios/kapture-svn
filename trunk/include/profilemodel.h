#ifndef PROFILEMODEL_H
#define PROFILEMODEL_H

#include <QAbstractTableModel>
#include <QList>

#include "profile.h"

class ProfileModel : public QAbstractTableModel
{
	Q_OBJECT
public:
	ProfileModel(QObject *parent = 0);
	~ProfileModel();
	void setProfileList(QList<Profile> p);
	void setData(QModelIndex index, QString value);
	QVariant data(const QModelIndex &index, int role) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	bool insertRow(int position, const QModelIndex &parent);
	bool removeRow(int position, const QModelIndex &parent);
private:
	QList<Profile> profiles;
};

#endif
