#include "preprocessorgridcrosssectionwindowdelegate.h"

#include <QSpinBox>
#include <QModelIndex>

#include <guibase/realnumbereditwidget.h>

QWidget* PreProcessorGridCrosssectionWindowDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem & /*option*/, const QModelIndex &index) const {
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Int){
		// Integer. Edit with QSpinBox
		QSpinBox* spinBox = new QSpinBox(parent);
		return spinBox;
	}else{
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = new RealNumberEditWidget(parent);
		return realEdit;
	}
}

void PreProcessorGridCrosssectionWindowDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Int){
		// Integer. Edit with QSpinBox
		QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
		spinBox->setValue(dat.toInt());
	}else{
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = static_cast<RealNumberEditWidget*>(editor);
		realEdit->setValue(dat.toDouble());
	}
}

void PreProcessorGridCrosssectionWindowDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
	QVariant dat = index.model()->data(index, Qt::DisplayRole);
	if (dat.type() == QVariant::Int){
		// Integer. editor = QSpinBox
		QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
		model->setData(index, spinBox->value(), Qt::EditRole);
	}else{
		// Float. Edit with RealEdit
		RealNumberEditWidget* realEdit = static_cast<RealNumberEditWidget*>(editor);
		model->setData(index, realEdit->value(), Qt::EditRole);
	}
}

void PreProcessorGridCrosssectionWindowDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex & index) const {
	editor->setGeometry(option.rect);
}
