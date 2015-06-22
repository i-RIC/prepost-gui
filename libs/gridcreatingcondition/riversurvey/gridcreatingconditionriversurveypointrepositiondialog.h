#ifndef GRIDCREATINGCONDITIONRIVERSURVEYPOINTREPOSITIONDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYPOINTREPOSITIONDIALOG_H

#include <QDialog>
#include <QAbstractButton>


#include <geodata/riversurvey/geodatariverpathpoint.h>

class GridCreatingConditionRiverSurvey;
//class GeoDataRiverPathPoint;

namespace Ui
{
	class GridCreatingConditionRiverSurveyPointRepositionDialog;
}

class GridCreatingConditionRiverSurveyPointRepositionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionRiverSurveyPointRepositionDialog(GridCreatingConditionRiverSurvey* gc, QWidget* parent = nullptr);
	~GridCreatingConditionRiverSurveyPointRepositionDialog();

	void apply();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	GeoDataRiverPathPoint::CtrlPointsAddMethod buildMethod();
	Ui::GridCreatingConditionRiverSurveyPointRepositionDialog* ui;

	GridCreatingConditionRiverSurvey* m_condition;
	GeoDataRiverPathPoint::CtrlPointsAddMethod m_method;
	bool m_applied;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYPOINTREPOSITIONDIALOG_H
