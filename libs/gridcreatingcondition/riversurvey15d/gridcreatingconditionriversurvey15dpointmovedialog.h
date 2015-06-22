#ifndef GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTMOVEDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTMOVEDIALOG_H

#include <geodata/riversurvey/geodatariverpathpoint.h>

#include <QDialog>
#include <QAbstractButton>

class GridCreatingConditionRiverSurvey15D;

namespace Ui
{
	class GridCreatingConditionRiverSurvey15DPointMoveDialog;
}

class GridCreatingConditionRiverSurvey15DPointMoveDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionRiverSurvey15DPointMoveDialog(
		GridCreatingConditionRiverSurvey15D* gc,
		double lowerLimit,
		double upperLimit,
		QWidget* parent = 0);
	~GridCreatingConditionRiverSurvey15DPointMoveDialog();

	void apply();
	void accept() override;
	void reject() override;

private:
	void setSValue();
	void doOffset();
	void doReset();

	GridCreatingConditionRiverSurvey15D* m_condition;
	double m_LowerLimit;
	double m_UpperLimit;
	double m_SValue;
	bool m_applied;

	Ui::GridCreatingConditionRiverSurvey15DPointMoveDialog* ui;

private slots:
	void handleButtonClick(QAbstractButton* button);
//	void on_buttonBox_clicked(QAbstractButton* button) override;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTMOVEDIALOG_H
