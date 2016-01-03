#include "ui_geodatariversurveydisplaysettingdialog.h"
#include "geodatariversurveydisplaysetting.h"
#include "geodatariversurveydisplaysettingdialog.h"

GeoDataRiverSurveyDisplaySettingDialog::GeoDataRiverSurveyDisplaySettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataRiverSurveyDisplaySettingDialog}
{
	ui->setupUi(this);
}

GeoDataRiverSurveyDisplaySettingDialog::~GeoDataRiverSurveyDisplaySettingDialog()
{
	delete ui;
}

GeoDataRiverSurveyDisplaySetting GeoDataRiverSurveyDisplaySettingDialog::setting() const
{
	GeoDataRiverSurveyDisplaySetting ret;

	ret.showBackground = ui->bgVisibleCheckBox->isChecked();
	ret.showLines = ui->clVisibleCheckBox->isChecked();
	ret.opacity = ui->transparencyWidget->opacityPercent();
	ret.crosssectionLinesScale = ui->zScaleSpinBox->value();
	ret.crosssectionLinesColor = ui->colorWidget->color();

	return ret;
}

void GeoDataRiverSurveyDisplaySettingDialog::setSetting(const GeoDataRiverSurveyDisplaySetting& setting)
{
	ui->bgVisibleCheckBox->setChecked(setting.showBackground);
	ui->clVisibleCheckBox->setChecked(setting.showLines);
	ui->transparencyWidget->setOpacityPercent(setting.opacity);
	ui->zScaleSpinBox->setValue(setting.crosssectionLinesScale);
	ui->colorWidget->setColor(setting.crosssectionLinesColor);
}
