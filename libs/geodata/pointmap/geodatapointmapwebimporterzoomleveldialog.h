#ifndef GEODATAPOINTMAPWEBIMPORTERZOOMLEVELDIALOG_H
#define GEODATAPOINTMAPWEBIMPORTERZOOMLEVELDIALOG_H

#include "geodatapointmapwebimportersetting.h"

#include <QDialog>

#include <vector>

namespace Ui {
class GeoDataPointmapWebImporterZoomLevelDialog;
}

class GeoDataPointmapWebImporterZoomLevelDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapWebImporterZoomLevelDialog(QWidget *parent = nullptr);
	~GeoDataPointmapWebImporterZoomLevelDialog();

	void setArea(double lonMin, double lonMax, double latMin, double latMax);

	int zoomLevel() const;
	QString url() const;

private slots:
	void updateResolution();
	void updateDataSize();
	void handleSourceChange(int source);

private:
	void updateList();

	Ui::GeoDataPointmapWebImporterZoomLevelDialog *ui;

	std::vector<GeoDataPointmapWebImporterSetting> m_settings;
	double m_latitude;
	double m_lonMin;
	double m_lonMax;
	double m_latMin;
	double m_latMax;
};

#endif // GEODATAPOINTMAPWEBIMPORTERZOOMLEVELDIALOG_H
