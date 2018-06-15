#ifndef GEODATAPOINTMAPWEBIMPORTERREGIONSELECTDIALOG_H
#define GEODATAPOINTMAPWEBIMPORTERREGIONSELECTDIALOG_H

#include <guicore/tmsimage/tmsimagesetting.h>

#include <QDialog>

#include <vector>

class CoordinateSystem;

namespace Ui {
class GeoDataPointmapWebImporterRegionSelectDialog;
}

class GeoDataPointmapWebImporterRegionSelectDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataPointmapWebImporterRegionSelectDialog(QWidget *parent = 0);
	~GeoDataPointmapWebImporterRegionSelectDialog();

	void setCoordinateSystem(CoordinateSystem* cs);

	double minLon() const;
	double maxLon() const;
	double minLat() const;
	double maxLat() const;

public slots:
	void accept() override;

private slots:
	void switchMap(int index);

private:
	std::vector<TmsImageSetting> m_settings;

	Ui::GeoDataPointmapWebImporterRegionSelectDialog *ui;
};

#endif // GEODATAPOINTMAPWEBIMPORTERREGIONSELECTDIALOG_H
