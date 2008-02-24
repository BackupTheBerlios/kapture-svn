#include <QPushButton>
#include "rosterdelegate.h"

ContactWidgetDelegate::ContactWidgetDelegate(QObject *parent)
	: QItemDelegate(parent)
{

}

ContactWidgetDelegate::~ContactWidgetDelegate()
{

}

QWidget *ContactWidgetDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QPushButton *btn = new QPushButton("Qt Rocks ! So does Kapture.", parent);
	return btn;
}

void ContactWidgetDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QPushButton *btn = static_cast<QPushButton*>(editor);
	btn->setText("Yes !");
}

void ContactWidgetDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

}

void ContactWidgetDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}

