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
	adjustSize();
}

void GridAttributeVariationEditDialog::setLabel(const QString& label)
{
	ui->label->setText(label);
}

void GridAttributeVariationEditDialog::applyVariation(GridAttributeContainer* container, QVector<vtkIdType>& indices, vtkDataSetAttributes* atts, PreProcessorGridDataItemInterface* dItem)
{
	m_widget->applyVariation(container, indices, atts, dItem);
}
