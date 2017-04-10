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

void GeoDataRiverSurveyImporterSettingDialog::setWith4Points(bool with4points)
{
	if (! with4points) {
		ui->middlePointRadioButton->setEnabled(true);
		ui->smallestRadioButton->setEnabled(true);
		ui->lowWaterWayRadioButton->setEnabled(false);

		ui->middlePointRadioButton->setChecked(true);
	} else {
		ui->middlePointRadioButton->setEnabled(false);
		ui->smallestRadioButton->setEnabled(false);
		ui->lowWaterWayRadioButton->setEnabled(true);

		ui->lowWaterWayRadioButton->setChecked(true);
	}
}

GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting GeoDataRiverSurveyImporterSettingDialog::centerPointSetting() const
{
	if (ui->middlePointRadioButton->isChecked()) {
		return cpMiddle;
	} else if (ui->smallestRadioButton->isChecked()) {
		return cpElevation;
	} else if (ui->lowWaterWayRadioButton->isChecked()) {
		return cpLowWaterWay;
	}
	return cpMiddle;
}
