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
	explicit GeoDataPointmapWebImporterZoomLevelDialog(QWidget *parent = 0);
	~GeoDataPointmapWebImporterZoomLevelDialog();

	void setCenterLatitude(double lat);

	int zoomLevel() const;
	QString url() const;

private slots:
	void updateResolution();
	void handleSourceChange(int source);

private:
	void updateList();

	Ui::GeoDataPointmapWebImporterZoomLevelDialog *ui;

	std::vector<GeoDataPointmapWebImporterSetting> m_settings;
	double m_latitude;
};

#endif // GEODATAPOINTMAPWEBIMPORTERZOOMLEVELDIALOG_H
