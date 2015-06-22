#ifndef GRIDCREATINGCONDITIONRIVERSURVEYPOINTREGIONADDDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYPOINTREGIONADDDIALOG_H

#include <QDialog>
#include <QList>

class GeoDataRiverSurvey;
class GeoDataRiverPathPoint;
class GridCreatingConditionRiverSurvey;

namespace Ui
{
	class GridCreatingConditionRiverSurveyPointRegionAddDialog;
}

class GridCreatingConditionRiverSurveyPointRegionAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionRiverSurveyPointRegionAddDialog(GridCreatingConditionRiverSurvey* cond, QWidget* parent = nullptr);
	~GridCreatingConditionRiverSurveyPointRegionAddDialog();
	void setData(GeoDataRiverSurvey* rs);
	void setStartPoint(GeoDataRiverPathPoint* p);
	void setEndPoint(GeoDataRiverPathPoint* p);
	GeoDataRiverPathPoint* startPoint();
	GeoDataRiverPathPoint* endPoint();

public slots:
	void accept() override;

private slots:
	void handleStartUpdate();
	void handleEndUpdate();

protected:
	void changeEvent(QEvent* e) override;

private:
	Ui::GridCreatingConditionRiverSurveyPointRegionAddDialog* ui;

	QList<GeoDataRiverPathPoint*> m_points;
	GridCreatingConditionRiverSurvey* m_condition;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYPOINTREGIONADDDIALOG_H
