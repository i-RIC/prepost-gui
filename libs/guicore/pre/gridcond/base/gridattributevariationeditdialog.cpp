#include "ui_gridattributevariationeditdialog.h"

#include "../../../solverdef/solverdefinitiongridattribute.h"
#include "gridattributevariationeditdialog.h"

#include <QString>

GridAttributeVariationEditDialog::GridAttributeVariationEditDialog(QWidget* parent) :
	QDialog(parent),
	m_widget {nullptr},
	ui(new Ui::GridAttributeVariationEditDialog)
{
	ui->setupUi(this);
}

GridAttributeVariationEditDialog::~GridAttributeVariationEditDialog()
{
	delete ui;
}

GridAttributeVariationEditWidget* GridAttributeVariationEditDialog::widget() const
{
	return m_widget;
}

void GridAttributeVariationEditDialog::setWidget(GridAttributeVariationEditWidget* w)
{
	m_widget = w;
	ui->widget->setWidget(w);
	QString label = w->gridAttribute()->caption();
	label.append(" :");
	ui->conditionLabel->setText(label);
	adjustSize();
}

void GridAttributeVariationEditDialog::clearValue()
{
	m_widget->clearValue();
}

void GridAttributeVariationEditDialog::setLabel(const QString& label)
{
	ui->label->setText(label);
}

void GridAttributeVariationEditDialog::applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem)
{
	m_widget->applyVariation(container, indices, atts, dItem);
}

QVariant GridAttributeVariationEditDialog::variantValue() const
{
	return m_widget->variantValue();
}

void GridAttributeVariationEditDialog::accept()
{
	QVariant v = m_widget->variantValue();
	QDialog::accept();
}

void GridAttributeVariationEditDialog::reject()
{
	QDialog::reject();
}
