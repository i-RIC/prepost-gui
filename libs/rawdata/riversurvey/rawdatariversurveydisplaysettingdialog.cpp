#include "rawdatariversurveydisplaysettingdialog.h"
#include "ui_rawdatariversurveydisplaysettingdialog.h"

RawDataRiverSurveyDisplaySettingDialog::RawDataRiverSurveyDisplaySettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::RawDataRiverSurveyDisplaySettingDialog)
{
	ui->setupUi(this);
}

RawDataRiverSurveyDisplaySettingDialog::~RawDataRiverSurveyDisplaySettingDialog()
{
	delete ui;
}

void RawDataRiverSurveyDisplaySettingDialog::setColormapVisible(bool visible)
{
	ui->bgVisibleCheckBox->setChecked(visible);
}

void RawDataRiverSurveyDisplaySettingDialog::setOpacityPercent(int opacity)
{
	ui->transparencyWidget->setOpacity(opacity);
}

bool RawDataRiverSurveyDisplaySettingDialog::colormapVisible()
{
	return ui->bgVisibleCheckBox->isChecked();
}

int RawDataRiverSurveyDisplaySettingDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

void RawDataRiverSurveyDisplaySettingDialog::setLinesVisible(bool visible)
{
	ui->clVisibleCheckBox->setChecked(visible);
}

void RawDataRiverSurveyDisplaySettingDialog::setLineColor(const QColor& color)
{
	ui->colorWidget->setColor(color);
}

void RawDataRiverSurveyDisplaySettingDialog::setZScale(int scale)
{
	ui->zScaleSpinBox->setValue(scale);
}

bool RawDataRiverSurveyDisplaySettingDialog::linesVisible()
{
	return ui->clVisibleCheckBox->isChecked();
}

int RawDataRiverSurveyDisplaySettingDialog::zScale()
{
	return ui->zScaleSpinBox->value();
}

QColor RawDataRiverSurveyDisplaySettingDialog::lineColor()
{
	return ui->colorWidget->color();
}
