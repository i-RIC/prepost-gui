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
}

GeoDataPointmapWebImporterZoomLevelDialog::~GeoDataPointmapWebImporterZoomLevelDialog()
{
	delete ui;
}

void GeoDataPointmapWebImporterZoomLevelDialog::setMaxZoomLevel(int level)
{
	ui->zoomLevelSpinBox->setMaximum(level);
	ui->zoomLevelSpinBox->setValue(level);
}

void GeoDataPointmapWebImporterZoomLevelDialog::setCenterLatitude(double lat)
{
	m_latitude = lat;
}

int GeoDataPointmapWebImporterZoomLevelDialog::zoomLevel() const
{
	return ui->zoomLevelSpinBox->value();
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
