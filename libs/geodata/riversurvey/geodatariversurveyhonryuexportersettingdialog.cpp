#include "geodatariversurveyhonryuexportersettingdialog.h"
#include "ui_geodatariversurveyhonryuexportersettingdialog.h"

GeoDataRiverSurveyHonryuExporterSettingDialog::GeoDataRiverSurveyHonryuExporterSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyHonryuExporterSettingDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurveyHonryuExporterSettingDialog::~GeoDataRiverSurveyHonryuExporterSettingDialog()
{
	delete ui;
}

void GeoDataRiverSurveyHonryuExporterSettingDialog::setCsvMode(bool csvMode)
{
	if (csvMode) {
		ui->commaRadioButton->setChecked(true);
		ui->spaceRadioButton->setEnabled(false);
		ui->tabRadioButton->setEnabled(false);
	} else {
		ui->spaceRadioButton->setChecked(true);
	}
}

GeoDataRiverSurveyHonryuExporterSettingDialog::Separator GeoDataRiverSurveyHonryuExporterSettingDialog::separator() const
{
	if (ui->commaRadioButton->isChecked()) {
		return Comma;
	} else if (ui->spaceRadioButton->isChecked()) {
		return Space;
	} else {
		return Tab;
	}
}

bool GeoDataRiverSurveyHonryuExporterSettingDialog::addHeader() const
{
	return ui->headerCheckBox->isChecked();
}

GeoDataRiverSurveyHonryuExporterSettingDialog::LongitudinalDistance GeoDataRiverSurveyHonryuExporterSettingDialog::longitudinalDistance() const
{
	if (ui->distanceComboBox->currentIndex() == 0) {
		return From_Previouos;
	} else {
		return From_First;
	}
}

GeoDataRiverSurveyHonryuExporterSettingDialog::Columns GeoDataRiverSurveyHonryuExporterSettingDialog::columns() const
{
	if (ui->columnsComboBox->currentIndex() == 0) {
		return Name_LongDistance_Distance_Elevation;
	} else if (ui->columnsComboBox->currentIndex() == 1) {
		return LongDistance_Name_Distance_Elevation;
	} else if (ui->columnsComboBox->currentIndex() == 2) {
		return Distance_Elevation_Name_LongDistance;
	} else {
		return Distance_Elevation_LongDistance_Name;
	}
}
