#ifndef GRIDCREATINGCONDITIONRIVERSURVEY15DREGIONDIALOG_H
#define GRIDCREATINGCONDITIONRIVERSURVEY15DREGIONDIALOG_H

#include <QDialog>
#include <QList>

class RawDataRiverSurvey;
class RawDataRiverPathPoint;
class GridCreatingConditionRiverSurvey15D;

namespace Ui {
	class GridCreatingConditionRiverSurvey15DRegionDialog;
}

class GridCreatingConditionRiverSurvey15DRegionDialog : public QDialog {
	Q_OBJECT
public:
	explicit GridCreatingConditionRiverSurvey15DRegionDialog(GridCreatingConditionRiverSurvey15D* cond, QWidget *parent = nullptr);
	~GridCreatingConditionRiverSurvey15DRegionDialog();
	void setData(RawDataRiverSurvey* rs);
	void setStartPoint(RawDataRiverPathPoint* p);
	void setEndPoint(RawDataRiverPathPoint* p);
	RawDataRiverPathPoint* startPoint();
	RawDataRiverPathPoint* endPoint();

public slots:
	void accept();

private slots:
	void handleStartUpdate();
	void handleEndUpdate();
	void setNumberSpinBox();

private:
	QList<RawDataRiverPathPoint*> m_points;
	GridCreatingConditionRiverSurvey15D* m_condition;

	Ui::GridCreatingConditionRiverSurvey15DRegionDialog *ui;
};

#endif // GRIDCREATINGCONDITIONRIVERSURVEY15DREGIONDIALOG_H
