#include "geodatariversurveygeneratedialog.h"
#include "ui_geodatariversurveygeneratedialog.h"

GeoDataRiverSurveyGenerateDialog::GeoDataRiverSurveyGenerateDialog(QWidget *parent) :
	QDialog(parent),
	m_centerLineLength {1},
	ui(new Ui::GeoDataRiverSurveyGenerateDialog)
{
	ui->setupUi(this);
	connect(ui->numXSecSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateCrossSectionDistance()));
}

GeoDataRiverSurveyGenerateDialog::~GeoDataRiverSurveyGenerateDialog()
{
	delete ui;
}

void GeoDataRiverSurveyGenerateDialog::setDEMDatas(std::vector<QString>& dataNames)
{
	ui->demDataComboBox->clear();
	for (const auto& name : dataNames) {
		ui->demDataComboBox->addItem(name);
	}
	ui->demDataComboBox->setCurrentIndex(0);
}

void GeoDataRiverSurveyGenerateDialog::setCenterLineLength(double len)
{
	m_centerLineLength = len;
	updateCrossSectionDistance();
}

int GeoDataRiverSurveyGenerateDialog::numberOfCrossSections() const
{
	return ui->numXSecSpinBox->value();
}

double GeoDataRiverSurveyGenerateDialog::divDistance() const
{
	return ui->distElevationSpinBox->value();
}

double GeoDataRiverSurveyGenerateDialog::upstreamName() const
{
	return ui->upstreamNameSpinBox->value();
}

int GeoDataRiverSurveyGenerateDialog::demData() const
{
	return ui->demDataComboBox->currentIndex();
}

void GeoDataRiverSurveyGenerateDialog::updateCrossSectionDistance()
{
	double dist = m_centerLineLength / (ui->numXSecSpinBox->value() - 1);
	ui->distSecValueLabel->setValue(dist);
}
