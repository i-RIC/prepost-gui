#ifndef GEODATARIVERSURVEYBACKGROUNDGRIDCREATETHREAD_H
#define GEODATARIVERSURVEYBACKGROUNDGRIDCREATETHREAD_H

#include "geodatariversurvey.h"

#include <QMap>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

class GeoDataRiverPathPoint;
class RiverGridCtrlSolver;

class GeoDataRiverSurveyBackgroundGridCreateThread : public QThread
{
	Q_OBJECT

public:
	GeoDataRiverSurveyBackgroundGridCreateThread(GeoDataRiverSurvey* parent);
	~GeoDataRiverSurveyBackgroundGridCreateThread();

	void update();
	void cancel();

	vtkStructuredGrid* grid() const;
	vtkPointSet* partialGrid(GeoDataRiverPathPoint* p) const;

	void setUseDivisionPoints(bool use);

	void startBGGridCopy();
	void finishBGGridCopy();

signals:
	void gridUpdated();

protected:
	void run() override;

private:
	bool runStandard();
	bool runUsingDivisionPoints();
	void updateGridInterpolators();
	vtkIdType gridIndex(int i, int j, int ISize);

	vtkSmartPointer<vtkStructuredGrid> m_grid;
	QMap<GeoDataRiverPathPoint*, vtkPointSet*> m_partialGrids;
	QMutex m_mutex;
	QWaitCondition m_runCondition;
	QWaitCondition m_cancelCondition;
	QWaitCondition m_bgGridCopyFinishCondition;

	QList<RiverGridCtrlSolver*> m_gridSolvers;

	bool m_useDivisionPoints;

	bool m_restart;
	bool m_canceled;
	bool m_abort;

	bool m_copyingBGGrid;
};

#endif // GEODATARIVERSURVEYBACKGROUNDGRIDCREATETHREAD_H
