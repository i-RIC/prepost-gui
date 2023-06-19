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
	ret.opacity = ui->transparencyWidget->opacity();
	ret.crosssectionLinesScale = ui->zScaleSpinBox->value();
	ret.crosssectionLinesColor = ui->colorWidget->color();
	ret.showNames = ui->nameVisibleCheckBox->isChecked();
	ret.namesTextSetting = ui->nameFontWidget->setting();

	return ret;
}

void GeoDataRiverSurveyDisplaySettingDialog::setSetting(const GeoDataRiverSurveyDisplaySetting& setting)
{
	ui->bgVisibleCheckBox->setChecked(setting.showBackground);
	ui->clVisibleCheckBox->setChecked(setting.showLines);
	ui->transparencyWidget->setOpacity(setting.opacity);
	ui->zScaleSpinBox->setValue(setting.crosssectionLinesScale);
	ui->colorWidget->setColor(setting.crosssectionLinesColor);
	ui->nameVisibleCheckBox->setChecked(setting.showNames);
	ui->nameFontWidget->setSetting(setting.namesTextSetting);
}
