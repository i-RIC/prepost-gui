#include "ui_gridcellattributepropertydialog.h"

#include "gridcellattributepropertydialog.h"

GridCellAttributePropertyDialog::GridCellAttributePropertyDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCellAttributePropertyDialog)
{
	ui->setupUi(this);
}

GridCellAttributePropertyDialog::~GridCellAttributePropertyDialog()
{
	delete ui;
}

void GridCellAttributePropertyDialog::setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget)
{
	ui->scalarsToColorsWidget->setWidget(widget);
	adjustSize();
}

ScalarsToColorsEditWidget* GridCellAttributePropertyDialog::scalarsToColorsEditWidget() const
{
	return ui->scalarsToColorsWidget->widget();
}

void GridCellAttributePropertyDialog::setOpacityPercent(int opacity)
{
	return ui->transparencyWidget->setOpacityPercent(opacity);
}

int GridCellAttributePropertyDialog::opacityPercent()
{
	return ui->transparencyWidget->opacityPercent();
}

void GridCellAttributePropertyDialog::accept()
{
	ui->scalarsToColorsWidget->save();
	QDialog::accept();
}
