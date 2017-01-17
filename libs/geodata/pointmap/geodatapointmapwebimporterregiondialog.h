#ifndef GEODATAPOINTMAPWEBIMPORTERREGIONDIALOG_H
#define GEODATAPOINTMAPWEBIMPORTERREGIONDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataPointmapWebImporterRegionDialog;
}

class GeoDataPointmapWebImporterRegionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapWebImporterRegionDialog(QWidget *parent = 0);
	~GeoDataPointmapWebImporterRegionDialog();

	void setMaxZoomLevel(int level);
	void setCenterLatitude(double lat);

	int zoomLevel() const;

private:
	void updateResolution();

private:
	Ui::GeoDataPointmapWebImporterRegionDialog *ui;

	bool m_latitude;
};

#endif // GEODATAPOINTMAPWEBIMPORTERREGIONDIALOG_H
