#ifndef GEODATAPOINTMAPWEBIMPORTERZOOMLEVELDIALOG_H
#define GEODATAPOINTMAPWEBIMPORTERZOOMLEVELDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataPointmapWebImporterZoomLevelDialog;
}

class GeoDataPointmapWebImporterZoomLevelDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapWebImporterZoomLevelDialog(QWidget *parent = 0);
	~GeoDataPointmapWebImporterZoomLevelDialog();

	void setMaxZoomLevel(int level);
	void setCenterLatitude(double lat);

	int zoomLevel() const;
	QString url() const;

private slots:
	void updateResolution();

private:
	Ui::GeoDataPointmapWebImporterZoomLevelDialog *ui;

	double m_latitude;
};

#endif // GEODATAPOINTMAPWEBIMPORTERZOOMLEVELDIALOG_H
