#ifndef GRIDCREATINGCONDITIONRIVERSURVEYPOINTREPOSITIONDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYPOINTREPOSITIONDIALOG_H

#include <QDialog>
#include <QAbstractButton>


#include <rawdata/riversurvey/rawdatariverpathpoint.h>

class GridCreatingConditionRiverSurvey;
//class RawDataRiverPathPoint;

namespace Ui {
	class GridCreatingConditionRiverSurveyPointRepositionDialog;
}

class GridCreatingConditionRiverSurveyPointRepositionDialog : public QDialog {
	Q_OBJECT
public:
	explicit GridCreatingConditionRiverSurveyPointRepositionDialog(GridCreatingConditionRiverSurvey* gc, QWidget *parent = nullptr);
	~GridCreatingConditionRiverSurveyPointRepositionDialog();

	void apply();
public slots:
	void accept();
	void reject();
private slots:
	void handleButtonClick(QAbstractButton* button);
private:
	RawDataRiverPathPoint::CtrlPointsAddMethod buildMethod();
	Ui::GridCreatingConditionRiverSurveyPointRepositionDialog *ui;

	GridCreatingConditionRiverSurvey* m_condition;
	RawDataRiverPathPoint::CtrlPointsAddMethod m_method;
	bool m_applied;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYPOINTREPOSITIONDIALOG_H
