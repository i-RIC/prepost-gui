#ifndef GEODATARIVERSURVEYBUILDBANKLINESDIALOG_H
#define GEODATARIVERSURVEYBUILDBANKLINESDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyBuildBankLinesDialog;
}

class GeoDataRiverSurveyBuildBankLinesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverSurveyBuildBankLinesDialog(QWidget *parent = 0);
	~GeoDataRiverSurveyBuildBankLinesDialog();

	double leftBankDistance();
	void setLeftBankDistance(double dis);

	double rightBankDistance();
	void setRightBankDistance(double dis);

private:
	Ui::GeoDataRiverSurveyBuildBankLinesDialog *ui;
};

#endif // GEODATARIVERSURVEYBUILDBANKLINESDIALOG_H
