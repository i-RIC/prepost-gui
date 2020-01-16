#include "../../widget/coordinateeditwidget.h"
#include "coordinateseditdialogdelegate.h"


#include <QPainter>

CoordinatesEditDialogDelegate::CoordinatesEditDialogDelegate(QObject* parent) :
	QStyledItemDelegate(parent)
{}

void CoordinatesEditDialogDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	double val = index.model()->data(index, Qt::DisplayRole).toDouble();
	CoordinateEditWidget widget;
	widget.setValue(val);

	QPixmap pixmap(option.rect.size());
	widget.render(&pixmap);
	painter->drawPixmap(option.rect, pixmap);
}

QWidget* CoordinatesEditDialogDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /*option*/, const QModelIndex & /*index*/) const
{
	return new CoordinateEditWidget(parent);
}

void CoordinatesEditDialogDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	auto widget = dynamic_cast<CoordinateEditWidget*>(editor);
	widget->setValue(dat.toDouble());
}

void CoordinatesEditDialogDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	auto widget = dynamic_cast<CoordinateEditWidget*>(editor);
	model->setData(index, widget->value(), Qt::EditRole);
}

void CoordinatesEditDialogDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}
