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

void GeoDataRiverSurveyGenerateDialog::setDEMDatas(std::vector<QString>& pointMapDataNames, std::vector<QString>& netcdfDataNames)
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

GeoDataRiverSurveyGenerateDialog::MappingTargetData GeoDataRiverSurveyGenerateDialog::mappingTargetData() const
{
	if (ui->pointMapRadioButton->isChecked()) {
		return MappingTargetData::PointCloud;
	} else if (ui->rasterRadioButton->isChecked()) {
		return MappingTargetData::Raster;
	}

	return MappingTargetData::PointCloud;
}

int GeoDataRiverSurveyGenerateDialog::dataId() const
{
	if (ui->pointMapRadioButton->isChecked()) {
		return ui->pointMapDataComboBox->currentIndex();
	}
	if (ui->rasterRadioButton->isChecked()) {
		return ui->rasterDataComboBox->currentIndex();
	}

	return 0;
}

void GeoDataRiverSurveyGenerateDialog::updateCrossSectionDistance()
{
	double dist = m_centerLineLength / (ui->numXSecSpinBox->value() - 1);
	ui->distSecValueLabel->setValue(dist);
}
