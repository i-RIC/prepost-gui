#ifndef GRIDCREATINGCONDITIONRIVERSURVEYPOINTMOVEDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYPOINTMOVEDIALOG_H

#include <QDialog>
#include <QAbstractButton>


#include <rawdata/riversurvey/rawdatariverpathpoint.h>

class GridCreatingConditionRiverSurvey;

namespace Ui {
	class GridCreatingConditionRiverSurveyPointMoveDialog;
}

class GridCreatingConditionRiverSurveyPointMoveDialog : public QDialog {
	Q_OBJECT
public:
	explicit GridCreatingConditionRiverSurveyPointMoveDialog(
		GridCreatingConditionRiverSurvey* gc,
		double lowerLimit, double upperLimit,
		QWidget *parent = 0);
	~GridCreatingConditionRiverSurveyPointMoveDialog();

	void apply();
	void accept();
	void reject();

private:
	void setSValue();
	void doOffset(bool preview = false);
	void doReset();

private:
	Ui::GridCreatingConditionRiverSurveyPointMoveDialog *ui;

	GridCreatingConditionRiverSurvey* m_condition;
	double m_LowerLimit;
	double m_UpperLimit;
	double m_SValue;

	bool m_applied;

private slots:
	void on_buttonBox_clicked(QAbstractButton* button);
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYPOINTMOVEDIALOG_H
