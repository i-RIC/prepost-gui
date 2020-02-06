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

void GeoDataRiverSurveyMapPointsDialog::setDEMDatas(std::vector<QString>& dataNames)
{
	ui->demDataComboBox->clear();
	for (const auto& name : dataNames) {
		ui->demDataComboBox->addItem(name);
	}
	ui->demDataComboBox->setCurrentIndex(0);
}

double GeoDataRiverSurveyMapPointsDialog::divDistance() const
{
	return ui->distElevationSpinBox->value();
}

int GeoDataRiverSurveyMapPointsDialog::demData() const
{
	return ui->demDataComboBox->currentIndex();
}
