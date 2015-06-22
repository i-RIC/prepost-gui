#include "ui_geodatariversurveydisplaysettingdialog.h"

#include "geodatariversurveydisplaysettingdialog.h"

GeoDataRiverSurveyDisplaySettingDialog::GeoDataRiverSurveyDisplaySettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyDisplaySettingDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurveyDisplaySettingDialog::~GeoDataRiverSurveyDisplaySettingDialog()
{
	delete ui;
}

void GeoDataRiverSurveyDisplaySettingDialog::setColormapVisible(bool visible)
{
	ui->bgVisibleCheckBox->setChecked(visible);
}

void GeoDataRiverSurveyDisplaySettingDialog::setOpacityPercent(int opacity)
{
	ui->transparencyWidget->setOpacity(opacity);
}

bool GeoDataRiverSurveyDisplaySettingDialog::colormapVisible()
{
	return ui->bgVisibleCheckBox->isChecked();
}

int GeoDataRiverSurveyDisplaySettingDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

void GeoDataRiverSurveyDisplaySettingDialog::setLinesVisible(bool visible)
{
	ui->clVisibleCheckBox->setChecked(visible);
}

void GeoDataRiverSurveyDisplaySettingDialog::setLineColor(const QColor& color)
{
	ui->colorWidget->setColor(color);
}

void GeoDataRiverSurveyDisplaySettingDialog::setZScale(int scale)
{
	ui->zScaleSpinBox->setValue(scale);
}

bool GeoDataRiverSurveyDisplaySettingDialog::linesVisible()
{
	return ui->clVisibleCheckBox->isChecked();
}

int GeoDataRiverSurveyDisplaySettingDialog::zScale()
{
	return ui->zScaleSpinBox->value();
}

QColor GeoDataRiverSurveyDisplaySettingDialog::lineColor()
{
	return ui->colorWidget->color();
}
