#include "postcalculatedresulteditdialog_tabledelegate.h"
#include "../postzonedatacontainer.h"

#include <guibase/widget/realnumbereditwidget.h>

#include <QLineEdit>
#include <QComboBox>

#include <vtkDataSetAttributes.h>

PostCalculatedResultEditDialog::TableDelegate::TableDelegate(QObject* parent) :
	QItemDelegate(parent),
	m_result {nullptr},
	m_zoneDataContainer {nullptr}
{}

void PostCalculatedResultEditDialog::TableDelegate::setCalculatedResult(PostCalculatedResult* result)
{
	m_result = result;
}

void PostCalculatedResultEditDialog::TableDelegate::setCalculatedResultIndex(int index)
{
	m_resultIndex = index;
}

void PostCalculatedResultEditDialog::TableDelegate::setZoneDataContainer(PostZoneDataContainer* c)
{
	m_zoneDataContainer = c;
}

QWidget* PostCalculatedResultEditDialog::TableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 0) {
		auto comboBox = new QComboBox(parent);
		auto atts = m_result->dataSetAttributes();
		for (vtkIdType i = 0; i < atts->GetNumberOfArrays(); ++i) {
			if (atts->GetArray(i)->GetNumberOfComponents() > 1) {continue;}
			comboBox->addItem(atts->GetArrayName(i));
		}
		for (int i = 0; i < m_zoneDataContainer->calculatedResults().size(); ++i) {
			if (i == m_resultIndex) {break;}

			auto result = m_zoneDataContainer->calculatedResults().at(i);
			if (m_result->dataType() != result->dataType()) {continue;}

			comboBox->addItem(result->name().c_str());
		}
		return comboBox;
	} else if (index.column() == 1) {
		return new QLineEdit(parent);
	} else {
		return new RealNumberEditWidget(parent);
	}
}

void PostCalculatedResultEditDialog::TableDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QVariant data = index.model()->data(index);
	if (index.column() == 0) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		comboBox->setCurrentText(data.toString());
	} else if (index.column() == 1) {
		auto lineEdit = dynamic_cast<QLineEdit*> (editor);
		lineEdit->setText(data.toString());
	} else {
		auto realEdit = dynamic_cast<RealNumberEditWidget*> (editor);
		realEdit->setValue(data.toDouble());
	}
}

void PostCalculatedResultEditDialog::TableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (index.column() == 0) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		model->setData(index, comboBox->currentText());
	} else if (index.column() == 1) {
		auto lineEdit = dynamic_cast<QLineEdit*> (editor);
		model->setData(index, lineEdit->text().trimmed());
	} else {
		auto realEdit = dynamic_cast<RealNumberEditWidget*> (editor);
		model->setData(index, realEdit->value());
	}
}

void PostCalculatedResultEditDialog::TableDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

/*
void PostCalculatedResultEditDialog::TableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{

}
*/
