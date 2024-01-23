#ifndef GEODATARIVERSURVEYMAPPOINTSDIALOG_H
#define GEODATARIVERSURVEYMAPPOINTSDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyMapPointsDialog;
}

class GeoDataRiverSurveyMapPointsDialog : public QDialog
{
	Q_OBJECT

public:
	enum class MappingTargetData {
		PointCloud,
		Raster
	};

	explicit GeoDataRiverSurveyMapPointsDialog(QWidget *parent = nullptr);
	~GeoDataRiverSurveyMapPointsDialog();

	void setDEMDatas(std::vector<QString>& pointMapNames, std::vector<QString>& rasterNames);

	double divDistance() const;
	MappingTargetData mappingTargetData() const;
	int dataId() const;

private:
	Ui::GeoDataRiverSurveyMapPointsDialog *ui;
};

#endif // GEODATARIVERSURVEYMAPPOINTSDIALOG_H
