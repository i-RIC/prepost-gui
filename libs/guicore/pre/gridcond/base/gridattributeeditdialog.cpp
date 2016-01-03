#include "ui_gridattributeeditdialog.h"

#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "gridattributeeditdialog.h"

#include <QString>

GridAttributeEditDialog::GridAttributeEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridAttributeEditDialog),
	m_cancelDisabled {false},
	m_widget {nullptr}
{
	ui->setupUi(this);
}

GridAttributeEditDialog::~GridAttributeEditDialog()
{
	delete ui;
}

void GridAttributeEditDialog::disableCancel()
{
	ui->buttonBox->setStandardButtons(QDialogButtonBox::Ok);
	m_cancelDisabled = true;
}

GridAttributeEditWidget* GridAttributeEditDialog::widget() const
{
	return m_widget;
}

void GridAttributeEditDialog::setWidget(GridAttributeEditWidget* w)
{
	m_widget = w;
	ui->widget->setWidget(w);
	QString label = w->gridAttribute()->caption();
	label.append(" :");
	ui->conditionLabel->setText(label);
	adjustSize();
}

void GridAttributeEditDialog::clearValue()
{
	m_widget->clearValue();
}

void GridAttributeEditDialog::accept()
{
	QVariant v = m_widget->variantValue();
	QDialog::accept();
}

void GridAttributeEditDialog::reject()
{
	if (m_cancelDisabled) {return;}
	QDialog::reject();
}

bool GridAttributeEditDialog::isValueSelected() const
{
	return m_widget->isValueSelected();
}

void GridAttributeEditDialog::setLabel(const QString& label)
{
	ui->label->setText(label);
}

void GridAttributeEditDialog::setVariantValue(const QVariant& v)
{
	m_widget->setVariantValue(v);
}

void GridAttributeEditDialog::scanAndSetDefault(GridAttributeContainer* container, QVector<vtkIdType>& indices)
{
	m_widget->scanAndSetDefault(container, indices);
}

void GridAttributeEditDialog::applyValue(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem)
{
	m_widget->applyValue(container, indices, atts, dItem);
}

QVariant GridAttributeEditDialog::variantValue() const
{
	return m_widget->variantValue();
}
