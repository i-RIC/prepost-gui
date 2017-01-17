#include "geodatapointmapwebimporterregiondialog.h"
#include "ui_geodatapointmapwebimporterregiondialog.h"

#include <cmath>

namespace {

// Meter per pixel at equator for zoom level 1 = 40075334.2563 / 512.;
const double METERPERPIXEL_AT_EQUATOR_ZOOMLEVEL1 = 78272.137219;

} // namespace

GeoDataPointmapWebImporterRegionDialog::GeoDataPointmapWebImporterRegionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmapWebImporterRegionDialog)
{
	ui->setupUi(this);
	connect(ui->zoomLevelSpinBox, SIGNAL(valueChanged(int)), this, SLOT(updateResolution()));
}

GeoDataPointmapWebImporterRegionDialog::~GeoDataPointmapWebImporterRegionDialog()
{
	delete ui;
}

void GeoDataPointmapWebImporterRegionDialog::setMaxZoomLevel(int level)
{
	ui->zoomLevelSpinBox->setMaximum(level);
	ui->zoomLevelSpinBox->setValue(level);
}

void GeoDataPointmapWebImporterRegionDialog::setCenterLatitude(double lat)
{
	m_latitude = lat;
}

int GeoDataPointmapWebImporterRegionDialog::zoomLevel() const
{
	return ui->zoomLevelSpinBox->value();
}

void GeoDataPointmapWebImporterRegionDialog::updateResolution()
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
