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
	explicit GeoDataRiverSurveyMapPointsDialog(QWidget *parent = 0);
	~GeoDataRiverSurveyMapPointsDialog();

	void setDEMDatas(std::vector<QString>& dataNames);

	double divDistance() const;
	int demData() const;

private:
	Ui::GeoDataRiverSurveyMapPointsDialog *ui;
};

#endif // GEODATARIVERSURVEYMAPPOINTSDIALOG_H
