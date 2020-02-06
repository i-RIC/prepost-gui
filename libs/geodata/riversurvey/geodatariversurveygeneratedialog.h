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
	explicit GeoDataRiverSurveyGenerateDialog(QWidget *parent = 0);
	~GeoDataRiverSurveyGenerateDialog();

	void setDEMDatas(std::vector<QString>& dataNames);
	void setCenterLineLength(double len);

	int numberOfCrossSections() const;
	double divDistance() const;
	double upstreamName() const;
	int demData() const;

private slots:
	void updateCrossSectionDistance();

private:
	double m_centerLineLength;
	Ui::GeoDataRiverSurveyGenerateDialog *ui;
};

#endif // GEODATARIVERSURVEYGENERATEDIALOG_H
