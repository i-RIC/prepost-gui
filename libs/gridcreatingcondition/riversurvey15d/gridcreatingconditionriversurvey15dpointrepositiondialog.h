#ifndef GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTREPOSITIONDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTREPOSITIONDIALOG_H

#include <geodata/riversurvey/geodatariverpathpoint.h>

#include <QDialog>
#include <QAbstractButton>

class GridCreatingConditionRiverSurvey15D;

namespace Ui
{
	class GridCreatingConditionRiverSurvey15DPointRepositionDialog;
}

class GridCreatingConditionRiverSurvey15DPointRepositionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionRiverSurvey15DPointRepositionDialog(GridCreatingConditionRiverSurvey15D* gc, QWidget* parent = nullptr);
	~GridCreatingConditionRiverSurvey15DPointRepositionDialog();
	void apply();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	GeoDataRiverPathPoint::CtrlPointsAddMethod buildMethod();

	GridCreatingConditionRiverSurvey15D* m_condition;
	GeoDataRiverPathPoint::CtrlPointsAddMethod m_method;
	bool m_applied;

	Ui::GridCreatingConditionRiverSurvey15DPointRepositionDialog* ui;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTREPOSITIONDIALOG_H
