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

ScalarsToColorsEditWidget* GridCellAttributePropertyDialog::scalarsToColorsEditWidget() const
{
	return ui->scalarsToColorsWidget->widget();
}

void GridCellAttributePropertyDialog::setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget)
{
	ui->scalarsToColorsWidget->setWidget(widget);
	adjustSize();
}

int GridCellAttributePropertyDialog::opacityPercent() const
{
	return ui->transparencyWidget->opacityPercent();
}

void GridCellAttributePropertyDialog::setOpacityPercent(int opacity)
{
	return ui->transparencyWidget->setOpacityPercent(opacity);
}

void GridCellAttributePropertyDialog::accept()
{
	ui->scalarsToColorsWidget->save();
	QDialog::accept();
}
