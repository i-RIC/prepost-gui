#include "geodatapointmapwebimportersettingmanager.h"
#include "geodatapointmapwebimporterzoomleveldialog.h"
#include "ui_geodatapointmapwebimporterzoomleveldialog.h"

#include <cmath>

namespace {

// Meter per pixel at equator for zoom level 1 = 40075334.2563 / 512.;
const double METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1 = 78272.137219;

} // namespace

GeoDataPointmapWebImporterZoomLevelDialog::GeoDataPointmapWebImporterZoomLevelDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmapWebImporterZoomLevelDialog)
{
	ui->setupUi(this);

	connect(ui->zoomLevelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateResolution()));
	connect(ui->sourceComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(handleSourceChange(int)));

	updateList();
}

GeoDataPointmapWebImporterZoomLevelDialog::~GeoDataPointmapWebImporterZoomLevelDialog()
{
	delete ui;
}

void GeoDataPointmapWebImporterZoomLevelDialog::setCenterLatitude(double lat)
{
	m_latitude = lat;
}

int GeoDataPointmapWebImporterZoomLevelDialog::zoomLevel() const
{
	return ui->zoomLevelSpinBox->value();
}

QString GeoDataPointmapWebImporterZoomLevelDialog::url() const
{
	auto s = m_settings.at(ui->sourceComboBox->currentIndex());
	return s.url();
}

void GeoDataPointmapWebImporterZoomLevelDialog::updateResolution()
{
	int targetZl = zoomLevel();
	int zl = 1;
	double meterPerPixel = METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1;
	meterPerPixel *= std::cos(m_latitude / 180.0 * 3.141592);

	while (zl < targetZl) {
		++ zl;
		meterPerPixel /= 2.0;
	}

	QString label = QString("%1 m").arg(meterPerPixel);
	ui->resolutionValueLabel->setText(label);
}

void GeoDataPointmapWebImporterZoomLevelDialog::handleSourceChange(int source)
{
	auto s = m_settings.at(source);
	ui->zoomLevelSpinBox->setMinimum(s.minZoomLevel());
	ui->zoomLevelSpinBox->setMaximum(s.maxZoomLevel());
	ui->zoomLevelSpinBox->setValue(s.maxZoomLevel());
}

void GeoDataPointmapWebImporterZoomLevelDialog::updateList()
{
	GeoDataPointmapWebImporterSettingManager manager;
	m_settings = manager.settings();

	for (const auto& s : m_settings) {
		ui->sourceComboBox->addItem(s.caption());
	}
}
