#include "structured15dgridwithcrosssectioncrosssectionwindowdelegate.h"

#include <guibase/widget/realnumbereditwidget.h>

Structured15DGridWithCrossSectionCrossSectionWindowDelegate::Structured15DGridWithCrossSectionCrossSectionWindowDelegate()
{}

QWidget* Structured15DGridWithCrossSectionCrossSectionWindowDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Double) {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = new RealNumberEditWidget(parent);
		return realEdit;
	}
	return QStyledItemDelegate::createEditor(parent, option, index);
}

void Structured15DGridWithCrossSectionCrossSectionWindowDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Double) {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = static_cast<RealNumberEditWidget*>(editor);
		realEdit->setValue(dat.toDouble());
	} else {
		QStyledItemDelegate::setEditorData(editor, index);
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QVariant dat = index.model()->data(index, Qt::DisplayPropertyRole);
	if (dat.type() == QVariant::Double) {
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = static_cast<RealNumberEditWidget*>(editor);
		model->setData(index, realEdit->value(), Qt::EditRole);
	} else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void Structured15DGridWithCrossSectionCrossSectionWindowDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& /*index*/) const
{
	editor->setGeometry(option.rect);
}
