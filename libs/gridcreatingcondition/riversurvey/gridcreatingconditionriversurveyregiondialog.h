#ifndef GRIDCREATINGCONDITIONRIVERSURVEYREGIONDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEYREGIONDIALOG_H

#include <QDialog>
#include <QList>

class GeoDataRiverSurvey;
class GeoDataRiverPathPoint;
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

	void setReadOnly(bool readOnly);
	void setData(GeoDataRiverSurvey* rs);

	GeoDataRiverPathPoint* startPoint();
	void setStartPoint(GeoDataRiverPathPoint* p);

	GeoDataRiverPathPoint* endPoint();
	void setEndPoint(GeoDataRiverPathPoint* p);

public slots:
	void accept() override;

private slots:
	void handleStartUpdate();
	void handleEndUpdate();

private:
	QList<GeoDataRiverPathPoint*> m_points;
	GridCreatingConditionRiverSurvey* m_condition;
	Ui::GridCreatingConditionRiverSurveyRegionDialog* ui;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEYREGIONDIALOG_H
