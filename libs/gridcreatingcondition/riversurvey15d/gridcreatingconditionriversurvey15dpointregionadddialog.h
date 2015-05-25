#ifndef GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTREGIONADDDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTREGIONADDDIALOG_H

#include <QDialog>
#include <QList>

class RawDataRiverSurvey;
class RawDataRiverPathPoint;
class GridCreatingConditionRiverSurvey15D;

namespace Ui
{
	class GridCreatingConditionRiverSurvey15DPointRegionAddDialog;
}

class GridCreatingConditionRiverSurvey15DPointRegionAddDialog : public QDialog
{
	Q_OBJECT
public:
	explicit GridCreatingConditionRiverSurvey15DPointRegionAddDialog(GridCreatingConditionRiverSurvey15D* cond, QWidget* parent = nullptr);
	~GridCreatingConditionRiverSurvey15DPointRegionAddDialog();
	void setData(RawDataRiverSurvey* rs);
	void setStartPoint(RawDataRiverPathPoint* p);
	void setEndPoint(RawDataRiverPathPoint* p);
	RawDataRiverPathPoint* startPoint();
	RawDataRiverPathPoint* endPoint();

public slots:
	void accept() override;

private slots:
	void handleStartUpdate();
	void handleEndUpdate();

private:
	QList<RawDataRiverPathPoint*> m_points;
	GridCreatingConditionRiverSurvey15D* m_condition;

	Ui::GridCreatingConditionRiverSurvey15DPointRegionAddDialog* ui;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY15DPOINTREGIONADDDIALOG_H
