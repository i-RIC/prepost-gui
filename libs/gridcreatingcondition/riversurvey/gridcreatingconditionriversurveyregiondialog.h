#ifndef GRIDCREATINGCONDITIONRIVERSURVEYREGIONDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYREGIONDIALOG_H

#include <QDialog>
#include <QList>

class RawDataRiverSurvey;
class RawDataRiverPathPoint;
class GridCreatingConditionRiverSurvey;

namespace Ui
{
	class GridCreatingConditionRiverSurveyRegionDialog;
}


class GridCreatingConditionRiverSurveyRegionDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionRiverSurveyRegionDialog(GridCreatingConditionRiverSurvey* cond, QWidget* parent = nullptr);
	~GridCreatingConditionRiverSurveyRegionDialog();
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
	GridCreatingConditionRiverSurvey* m_condition;
	Ui::GridCreatingConditionRiverSurveyRegionDialog* ui;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYREGIONDIALOG_H
