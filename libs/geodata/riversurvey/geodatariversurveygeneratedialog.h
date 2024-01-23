#ifndef GEODATARIVERSURVEYGENERATEDIALOG_H
#define GEODATARIVERSURVEYGENERATEDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyGenerateDialog;
}

class GeoDataRiverSurveyGenerateDialog : public QDialog
{
	Q_OBJECT

public:
	enum class MappingTargetData {
		PointCloud,
		Raster
	};

	explicit GeoDataRiverSurveyGenerateDialog(QWidget *parent = nullptr);
	~GeoDataRiverSurveyGenerateDialog();

	void setDEMDatas(std::vector<QString>& pointMapNames, std::vector<QString>& rasterNames);
	void setCenterLineLength(double len);

	int numberOfCrossSections() const;
	double divDistance() const;
	double upstreamName() const;

	MappingTargetData mappingTargetData() const;
	int dataId() const;

private slots:
	void updateCrossSectionDistance();

private:
	double m_centerLineLength;
	Ui::GeoDataRiverSurveyGenerateDialog *ui;
};

#endif // GEODATARIVERSURVEYGENERATEDIALOG_H
