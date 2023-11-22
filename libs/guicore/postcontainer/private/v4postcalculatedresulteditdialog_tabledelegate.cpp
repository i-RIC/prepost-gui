#include "../../solverdef/solverdefinitiongridtype.h"
#include "../v4postzonedatacontainer.h"
#include "v4postcalculatedresulteditdialog_tabledelegate.h"

#include <guibase/widget/realnumbereditwidget.h>
#include <misc/stringtool.h>

#include <QLineEdit>
#include <QComboBox>

#include <vtkDataSetAttributes.h>

namespace {

std::vector<std::string> buildResultNames(vtkDataSetAttributes* atts) {
	std::vector<std::string> ret;

	for (vtkIdType i = 0; i < atts->GetNumberOfArrays(); ++i) {
		if (atts->GetArray(i)->GetNumberOfComponents() > 1) {continue;}

		ret.push_back(atts->GetArrayName(i));
	}

	return ret;
}

} // namespace

v4PostCalculatedResultEditDialog::TableDelegate::TableDelegate(QObject* parent) :
	QItemDelegate(parent),
	m_result {nullptr},
	m_zoneDataContainer {nullptr}
{}

void v4PostCalculatedResultEditDialog::TableDelegate::setCalculatedResult(v4PostCalculatedResult* result)
{
	m_result = result;
}

void v4PostCalculatedResultEditDialog::TableDelegate::setCalculatedResultIndex(int index)
{
	m_resultIndex = index;
}

void v4PostCalculatedResultEditDialog::TableDelegate::setZoneDataContainer(v4PostZoneDataContainer* c)
{
	m_zoneDataContainer = c;
}

QWidget* v4PostCalculatedResultEditDialog::TableDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 0) {
		auto gType = m_zoneDataContainer->gridType();

		auto comboBox = new QComboBox(parent);
		auto resultNames = buildResultNames(m_result->dataSetAttributes());

		for (const auto& name : resultNames) {
			comboBox->addItem(gType->outputCaption(name));
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

void v4PostCalculatedResultEditDialog::TableDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QVariant data = index.model()->data(index);
	if (index.column() == 0) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		auto resultNames = buildResultNames(m_result->dataSetAttributes());
		auto it = std::find(resultNames.begin(), resultNames.end(), iRIC::toStr(data.toString()));
		if (it != resultNames.end()) {
			comboBox->setCurrentIndex(it - resultNames.begin());
		} else {
			comboBox->setCurrentText(data.toString());
		}
	} else if (index.column() == 1) {
		auto lineEdit = dynamic_cast<QLineEdit*> (editor);
		lineEdit->setText(data.toString());
	} else {
		auto realEdit = dynamic_cast<RealNumberEditWidget*> (editor);
		realEdit->setValue(data.toDouble());
	}
}

void v4PostCalculatedResultEditDialog::TableDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (index.column() == 0) {
		auto comboBox = dynamic_cast<QComboBox*> (editor);
		auto resultNames = buildResultNames(m_result->dataSetAttributes());
		if (comboBox->currentIndex() < resultNames.size()) {
			model->setData(index, resultNames.at(comboBox->currentIndex()).c_str());
		} else {
			model->setData(index, comboBox->currentText());
		}
	} else if (index.column() == 1) {
		auto lineEdit = dynamic_cast<QLineEdit*> (editor);
		model->setData(index, lineEdit->text().trimmed());
	} else {
		auto realEdit = dynamic_cast<RealNumberEditWidget*> (editor);
		model->setData(index, realEdit->value());
	}
}

void v4PostCalculatedResultEditDialog::TableDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

/*
void v4PostCalculatedResultEditDialog::TableDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{

}
*/
