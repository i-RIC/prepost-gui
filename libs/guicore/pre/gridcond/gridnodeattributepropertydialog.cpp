#include "ui_gridnodeattributepropertydialog.h"

#include "../../scalarstocolors/lookuptableeditwidget.h"
#include "gridnodeattributepropertydialog.h"

GridNodeAttributePropertyDialog::GridNodeAttributePropertyDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridNodeAttributePropertyDialog)
{
	ui->setupUi(this);
	ui->contourSettingWidget->hidePointsRadioButton();
}

GridNodeAttributePropertyDialog::~GridNodeAttributePropertyDialog()
{
	delete ui;
}

void GridNodeAttributePropertyDialog::setContour(ContourSettingWidget::Contour contour)
{
	ui->contourSettingWidget->setContour(contour);
}

ContourSettingWidget::Contour GridNodeAttributePropertyDialog::contour()
{
	return ui->contourSettingWidget->contour();
}

int GridNodeAttributePropertyDialog::numberOfDivision()
{
	ScalarsToColorsEditWidget* w = ui->scalarsToColorsWidget->widget();
	LookupTableEditWidget* widget = dynamic_cast<LookupTableEditWidget*>(w);
	if (widget != nullptr) {
		return widget->divisionNumber();
	} else {
		return 10;
	}
}

void GridNodeAttributePropertyDialog::setNumberOfDivision(int n)
{
	ScalarsToColorsEditWidget* w = ui->scalarsToColorsWidget->widget();
	LookupTableEditWidget* widget = dynamic_cast<LookupTableEditWidget*>(w);
	if (widget != nullptr) {
		return widget->setDivisionNumber(n);
	}
}

void GridNodeAttributePropertyDialog::setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget)
{
	ui->scalarsToColorsWidget->setWidget(widget);
	LookupTableEditWidget* ltWidget = dynamic_cast<LookupTableEditWidget*>(widget);
	if (ltWidget != nullptr) {
		connect(ui->contourSettingWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), ltWidget, SLOT(setContourSetting(ContourSettingWidget::Contour)));
	}

	adjustSize();
}

void GridNodeAttributePropertyDialog::setOpacityPercent(int opacity)
{
	return ui->transparencyWidget->setOpacityPercent(opacity);
}

int GridNodeAttributePropertyDialog::opacityPercent()
{
	return ui->transparencyWidget->opacityPercent();
}

void GridNodeAttributePropertyDialog::handleContourChange()
{
	if (ui->contourSettingWidget->contour() == ContourSettingWidget::Isolines) {
		ui->transparencyWidget->setDisabled(true);
	} else {
		ui->transparencyWidget->setEnabled(true);
	}
}

void GridNodeAttributePropertyDialog::accept()
{
	ui->scalarsToColorsWidget->save();
	QDialog::accept();
}
