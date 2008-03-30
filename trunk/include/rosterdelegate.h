#ifndef ROSTERDELEGATE_H
#define ROSTERDELEGATE_H

#include <QItemDelegate>

class ContactWidgetDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	ContactWidgetDelegate(QObject *parent = 0);
	~ContactWidgetDelegate();

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif //ROSTERDELEGATE_H
