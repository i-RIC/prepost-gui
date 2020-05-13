#include "verticalcrosssectionwindowelevationsdockwidgetview_itemdelegete.h"

#include <QDoubleSpinBox>

VerticalCrossSectionWindowElevationsDockWidgetView::ItemDelegete::ItemDelegete(QObject* parent) :
	QStyledItemDelegate(parent)
{}

QWidget* VerticalCrossSectionWindowElevationsDockWidgetView::ItemDelegete::createEditor(QWidget* parent, const QStyleOptionViewItem&, const QModelIndex& index) const
{
	if (index.column() == 0) {
		return nullptr;
	} else {
		auto spinBox = new QDoubleSpinBox(parent);
		spinBox->setDecimals(3);
		spinBox->setSingleStep(0.01);
		spinBox->setMinimum(-1000);
		spinBox->setMaximum(10000);
		return spinBox;
	}
}

void VerticalCrossSectionWindowElevationsDockWidgetView::ItemDelegete::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (index.column() != 1) {return;}

	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	auto sb = dynamic_cast<QDoubleSpinBox*> (editor);
	sb->setValue(dat.toDouble());
}

void VerticalCrossSectionWindowElevationsDockWidgetView::ItemDelegete::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (index.column() != 1) {return;}

	auto sb = dynamic_cast<QDoubleSpinBox*> (editor);
	model->setData(index, sb->value(), Qt::EditRole);
}

void VerticalCrossSectionWindowElevationsDockWidgetView::ItemDelegete::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex&) const
{
	editor->setGeometry(option.rect);
}
