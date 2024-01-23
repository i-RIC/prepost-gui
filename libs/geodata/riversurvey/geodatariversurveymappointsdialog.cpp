#include "geodatariversurveymappointsdialog.h"
#include "ui_geodatariversurveymappointsdialog.h"

GeoDataRiverSurveyMapPointsDialog::GeoDataRiverSurveyMapPointsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyMapPointsDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurveyMapPointsDialog::~GeoDataRiverSurveyMapPointsDialog()
{
	delete ui;
}

void GeoDataRiverSurveyMapPointsDialog::setDEMDatas(std::vector<QString>& pointMapDataNames, std::vector<QString>& netcdfDataNames)
{
	ui->pointMapDataComboBox->clear();
	for (const auto& name : pointMapDataNames) {
		ui->pointMapDataComboBox->addItem(name);
	}
	if (pointMapDataNames.size() > 0) {
		ui->pointMapDataComboBox->setCurrentIndex(0);
	} else {
		ui->rasterRadioButton->setChecked(true);
		ui->pointMapRadioButton->setDisabled(true);
	}

	ui->rasterDataComboBox->clear();
	for (const auto& name : netcdfDataNames) {
		ui->rasterDataComboBox->addItem(name);
	}
	if (netcdfDataNames.size() > 0) {
		ui->rasterDataComboBox->setCurrentIndex(0);
	} else {
		ui->rasterRadioButton->setDisabled(true);
	}
}

double GeoDataRiverSurveyMapPointsDialog::divDistance() const
{
	return ui->distElevationSpinBox->value();
}

GeoDataRiverSurveyMapPointsDialog::MappingTargetData GeoDataRiverSurveyMapPointsDialog::mappingTargetData() const
{
	if (ui->pointMapRadioButton->isChecked()) {
		return MappingTargetData::PointCloud;
	} else if (ui->rasterRadioButton->isChecked()) {
		return MappingTargetData::Raster;
	}

	return MappingTargetData::PointCloud;
}

int GeoDataRiverSurveyMapPointsDialog::dataId() const
{
	if (ui->pointMapRadioButton->isChecked()) {
		return ui->pointMapDataComboBox->currentIndex();
	}
	if (ui->rasterRadioButton->isChecked()) {
		return ui->rasterDataComboBox->currentIndex();
	}

	return 0;
}
