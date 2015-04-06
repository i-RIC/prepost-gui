#ifndef RAWDATARIVERSURVEYBACKGROUNDGRIDCREATETHREAD_H
#define RAWDATARIVERSURVEYBACKGROUNDGRIDCREATETHREAD_H

#include "rawdatariversurvey.h"
#include <QThread>
#include <QMutex>
#include <QMap>
#include <QWaitCondition>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>
#include <vtkUnstructuredGrid.h>

class RawDataRiverPathPoint;
class RiverGridCtrlSolver;

class RawDataRiverSurveyBackgroundGridCreateThread : public QThread
{
	Q_OBJECT
private:
	static const int IDIVNUM = 4;
	static const int JDIVNUM = 20;
public:
	RawDataRiverSurveyBackgroundGridCreateThread(RawDataRiverSurvey* parent);
	~RawDataRiverSurveyBackgroundGridCreateThread();

	void update();
	void cancel();
	vtkStructuredGrid* grid(){return m_grid;}
	vtkPointSet* partialGrid(RawDataRiverPathPoint* p){
		return m_partialGrids.value(p, 0);
	}
	void setUseDivisionPoints(bool use){
		m_useDivisionPoints = use;
	}
	void startBGGridCopy();
	void finishBGGridCopy();

signals:
	void gridUpdated();
protected:
	void run();
private:
	bool runStandard();
	bool runUsingDivisionPoints();
	void updateGridInterpolators();
	vtkIdType gridIndex(int i, int j, int ISize);
	vtkSmartPointer<vtkStructuredGrid> m_grid;
	QMap<RawDataRiverPathPoint*, vtkPointSet*> m_partialGrids;
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

#endif // RAWDATARIVERSURVEYBACKGROUNDGRIDCREATETHREAD_H
