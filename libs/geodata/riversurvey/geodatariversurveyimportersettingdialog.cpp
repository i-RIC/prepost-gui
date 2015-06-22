#include "ui_geodatariversurveyimportersettingdialog.h"

#include "geodatariversurveyimportersettingdialog.h"

GeoDataRiverSurveyImporterSettingDialog::GeoDataRiverSurveyImporterSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyImporterSettingDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurveyImporterSettingDialog::~GeoDataRiverSurveyImporterSettingDialog()
{
	delete ui;
}

GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting GeoDataRiverSurveyImporterSettingDialog::centerPointSetting() const
{
	if (ui->middlePointRadioButton->isChecked()) {
		return cpMiddle;
	} else if (ui->smallestRadioButton->isChecked()) {
		return cpElevation;
	}
	return cpMiddle;
}
