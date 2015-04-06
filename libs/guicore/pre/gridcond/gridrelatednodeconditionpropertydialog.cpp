#include "gridrelatednodeconditionpropertydialog.h"
#include "ui_gridrelatednodeconditionpropertydialog.h"
#include "../../scalarstocolors/lookuptableeditwidget.h"

GridRelatedNodeConditionPropertyDialog::GridRelatedNodeConditionPropertyDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridRelatedNodeConditionPropertyDialog)
{
	ui->setupUi(this);
	ui->contourSettingWidget->hidePoints();
}

GridRelatedNodeConditionPropertyDialog::~GridRelatedNodeConditionPropertyDialog()
{
	delete ui;
}

void GridRelatedNodeConditionPropertyDialog::setContour(ContourSettingWidget::Contour contour)
{
	ui->contourSettingWidget->setContour(contour);
}

ContourSettingWidget::Contour GridRelatedNodeConditionPropertyDialog::contour()
{
	return ui->contourSettingWidget->contour();
}

int GridRelatedNodeConditionPropertyDialog::numberOfDivision()
{
	ScalarsToColorsEditWidget* w = ui->scalarsToColorsWidget->widget();
	LookupTableEditWidget* widget = dynamic_cast<LookupTableEditWidget*>(w);
	if (widget != 0){
		return widget->divisionNumber();
	} else {
		return 10;
	}
}

void GridRelatedNodeConditionPropertyDialog::setNumberOfDivision(int n)
{
	ScalarsToColorsEditWidget* w = ui->scalarsToColorsWidget->widget();
	LookupTableEditWidget* widget = dynamic_cast<LookupTableEditWidget*>(w);
	if (widget != 0){
		return widget->setDivisionNumber(n);
	}
}

void GridRelatedNodeConditionPropertyDialog::setScalarsToColorsEditWidget(ScalarsToColorsEditWidget* widget)
{
	ui->scalarsToColorsWidget->setWidget(widget);
	LookupTableEditWidget* ltWidget = dynamic_cast<LookupTableEditWidget*>(widget);
	if (ltWidget != 0){
		connect(ui->contourSettingWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), ltWidget, SLOT(setContourSetting(ContourSettingWidget::Contour)));
	}

	adjustSize();
}

void GridRelatedNodeConditionPropertyDialog::setOpacityPercent(int opacity)
{
	return ui->transparencyWidget->setOpacity(opacity);
}

int GridRelatedNodeConditionPropertyDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

void GridRelatedNodeConditionPropertyDialog::handleContourChange()
{
	if (ui->contourSettingWidget->contour() == ContourSettingWidget::Isolines){
		ui->transparencyWidget->setDisabled(true);
	}else{
		ui->transparencyWidget->setEnabled(true);
	}
}

void GridRelatedNodeConditionPropertyDialog::accept()
{
	ui->scalarsToColorsWidget->save();
	QDialog::accept();
}
