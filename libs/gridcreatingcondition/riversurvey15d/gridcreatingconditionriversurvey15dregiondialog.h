#ifndef GRIDCREATINGCONDITIONRIVERSURVEY15DREGIONDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEY15DREGIONDIALOG_H

#include <QDialog>
#include <QList>

class GeoDataRiverSurvey;
class GeoDataRiverPathPoint;
class GridCreatingConditionRiverSurvey15D;

namespace Ui
{
	class GridCreatingConditionRiverSurvey15DRegionDialog;
}

class GridCreatingConditionRiverSurvey15DRegionDialog : public QDialog
{
	Q_OBJECT

public:
	enum class PositionMode {LeftBank, CenterPoint};

	GridCreatingConditionRiverSurvey15DRegionDialog(GridCreatingConditionRiverSurvey15D* cond, QWidget* parent = nullptr);
	~GridCreatingConditionRiverSurvey15DRegionDialog();

	void setData(GeoDataRiverSurvey* rs);
	void setStartPoint(GeoDataRiverPathPoint* p);
	void setEndPoint(GeoDataRiverPathPoint* p);
	void setPositionMode(PositionMode mode);

	GeoDataRiverPathPoint* startPoint() const;
	GeoDataRiverPathPoint* endPoint() const;
	PositionMode positionMode() const;

public slots:
	void accept() override;

private slots:
	void handleStartUpdate();
	void handleEndUpdate();
	void setNumberSpinBox();

private:
	QList<GeoDataRiverPathPoint*> m_points;
	GridCreatingConditionRiverSurvey15D* m_condition;

	Ui::GridCreatingConditionRiverSurvey15DRegionDialog* ui;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY15DREGIONDIALOG_H
