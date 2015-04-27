#ifndef GRIDCREATINGCONDITIONRIVERSURVEYPOINTADDDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYPOINTADDDIALOG_H

#include <QDialog>
#include <rawdata/riversurvey/rawdatariverpathpoint.h>

namespace Ui {
	class GridCreatingConditionRiverSurveyPointAddDialog;
}

class GridCreatingConditionRiverSurvey;
class QAbstractButton;

class GridCreatingConditionRiverSurveyPointAddDialog : public QDialog {
	Q_OBJECT
public:
	GridCreatingConditionRiverSurveyPointAddDialog(GridCreatingConditionRiverSurvey* cond, QWidget *parent = nullptr);
	~GridCreatingConditionRiverSurveyPointAddDialog();
public slots:
	void accept();
	void reject();
private slots:
	void handleButtonClick(QAbstractButton* button);
private:
	bool m_applied;
	void apply();
	RawDataRiverPathPoint::CtrlPointsAddMethod buildMethod();
	GridCreatingConditionRiverSurvey* m_condition;
	Ui::GridCreatingConditionRiverSurveyPointAddDialog *ui;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYPOINTADDDIALOG_H
