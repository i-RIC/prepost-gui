#include "gridrelatedcellconditionpropertydialog.h"
#include "ui_gridrelatedcellconditionpropertydialog.h"

GridRelatedCellConditionPropertyDialog::GridRelatedCellConditionPropertyDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridRelatedCellConditionPropertyDialog)
{
	ui->setupUi(this);
}

GridRelatedCellConditionPropertyDialog::~GridRelatedCellConditionPropertyDialog()
{
	delete ui;
}

void GridRelatedCellConditionPropertyDialog::setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget)
{
	ui->scalarsToColorsWidget->setWidget(widget);
	adjustSize();
}

ScalarsToColorsEditWidget* GridRelatedCellConditionPropertyDialog::scalarsToColorsEditWidget() const
{
	return ui->scalarsToColorsWidget->widget();
}

void GridRelatedCellConditionPropertyDialog::setOpacityPercent(int opacity)
{
	return ui->transparencyWidget->setOpacity(opacity);
}

int GridRelatedCellConditionPropertyDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

void GridRelatedCellConditionPropertyDialog::accept()
{
	ui->scalarsToColorsWidget->save();
	QDialog::accept();
}
