#include "rawdatarivercrosssection.h"
#include "rawdatariverpathpoint.h"
#include "rawdatarivershapeinterpolator.h"
#include "rawdatariversurveybackgroundgridcreatethread.h"

#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QList>
#include <QVector2D>

#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkQuad.h>

#define DATA "Data"

RawDataRiverSurveyBackgroundGridCreateThread::RawDataRiverSurveyBackgroundGridCreateThread(RawDataRiverSurvey* rs)
	: QThread(rs)
{
	m_useDivisionPoints = false;

	m_restart = false;
	m_abort = false;
	m_canceled = false;
	m_copyingBGGrid = false;

	m_grid = vtkSmartPointer<vtkStructuredGrid>::New();
}

RawDataRiverSurveyBackgroundGridCreateThread::~RawDataRiverSurveyBackgroundGridCreateThread()
{
	m_mutex.lock();
	m_abort = true;
	m_mutex.unlock();

	wait();
}

void RawDataRiverSurveyBackgroundGridCreateThread::update(){
	QMutexLocker locker(&m_mutex);
	if (! isRunning()){
		start(LowestPriority);
	}else{
		m_restart = true;
		m_runCondition.wait(&m_mutex);
	}
}

void RawDataRiverSurveyBackgroundGridCreateThread::cancel(){
	QMutexLocker locker(&m_mutex);
	m_canceled = true;
	m_cancelCondition.wait(&m_mutex);
}

vtkIdType RawDataRiverSurveyBackgroundGridCreateThread::gridIndex(int i, int j, int ISize)
{
	return ISize * j + i;
}

void RawDataRiverSurveyBackgroundGridCreateThread::run()
{
	forever {
		// delete partial grids.
		m_mutex.lock();
		if (m_copyingBGGrid){
			m_bgGridCopyFinishCondition.wait(&m_mutex);
		}
		m_mutex.unlock();
		for (auto it = m_partialGrids.begin(); it != m_partialGrids.end(); ++it){
			vtkPointSet* tmpgrid = it.value();
			tmpgrid->Delete();
		}
		m_partialGrids.clear();

		bool ok;
		if (m_useDivisionPoints){
			ok = runUsingDivisionPoints();
		} else {
			ok = runStandard();
		}
		if (! ok){return;}

		m_mutex.lock();
		if (m_canceled){
			m_canceled = false;
			m_cancelCondition.wakeOne();
		}
		m_mutex.unlock();
		while (1){
			QMutexLocker locker(&m_mutex);
			if (m_abort){
				return;
			}
			if (m_canceled){
				m_canceled = false;
				m_cancelCondition.wakeOne();
			}
			if (m_restart){
				// restart requested!
				m_restart = false;
				break;
			}
			locker.unlock();
			// check every 10 ms.
			msleep(10);
		}
		// running started.
		m_runCondition.wakeOne();

		// when restart is requested, comes here.
		// wait for 500 ms before starting the process.
		for (int i = 0; i < 5 && ! m_canceled; ++i){
			msleep(10);
		}
	}
}

bool RawDataRiverSurveyBackgroundGridCreateThread::runStandard()
{
	int pointCount = 0;
	RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(parent());
	RawDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	while (p != 0){
		++pointCount;
		p = p->nextPoint();
	}
	// calculate gridsize.
	int gridISize = pointCount + (pointCount - 1) * (IDIVNUM - 1);
	int gridJSize = JDIVNUM * 2 + 1;

	// build interpolators first.
	QList<RiverBackgroundGridCtrlSolver*> solvers;
	for (int i = 0; i < JDIVNUM - 1; ++i){
		RiverBackgroundGridCtrlSolver* solver = new RiverBackgroundGridCtrlSolver();
		solver->SetBankSide(RiverBackgroundGridCtrlSolver::bs_LeftBank);
		solver->SetIndex(i);
		solver->setParameter((i + 1) / static_cast<double>(JDIVNUM));
		solver->setHeadPoint(rs->headPoint());
		solvers.append(solver);
	}
	for (int i = 0; i < JDIVNUM - 1; ++i){
		RiverBackgroundGridCtrlSolver* solver = new RiverBackgroundGridCtrlSolver();
		solver->SetBankSide(RiverBackgroundGridCtrlSolver::bs_RightBank);
		solver->SetIndex(i);
		solver->setParameter((i + 1) / static_cast<double>(JDIVNUM));
		solver->setHeadPoint(rs->headPoint());
		solvers.append(solver);
	}
	// setup interpolators.
	p = rs->headPoint();
	while (p != 0){
		// check condition to exit.
		if (m_abort){return false;}

		// check conditions to go out.
		if (m_restart){break;}
		if (m_canceled){break;}
		if (! m_useDivisionPoints){
			if (p->backgroundLGridLines().size() != JDIVNUM - 1){
				for (auto it = p->backgroundLGridLines().begin(); it != p->backgroundLGridLines().end(); ++it){
					delete *it;
				}
				p->backgroundLGridLines().clear();
				p->backgroundLGridLines().insert(0, JDIVNUM - 1, 0);
			}
			if (p->backgroundRGridLines().size() != JDIVNUM - 1){
				for (auto it = p->backgroundRGridLines().begin(); it != p->backgroundRGridLines().end(); ++it){
					delete *it;
				}
				p->backgroundRGridLines().clear();
				p->backgroundRGridLines().insert(0, JDIVNUM - 1, 0);
			}
		}
		p = p->nextPoint();
	}
	// build background interpolators.
	for (auto s_it = solvers.begin(); s_it != solvers.end(); ++s_it){
		// check condition to exit.
		if (m_abort){return false;}

		// check conditions to go out.
		if (m_restart){break;}
		if (m_canceled){break;}

		(*s_it)->update();
	}
	// initializes grid.
	m_grid = vtkSmartPointer<vtkStructuredGrid>::New();
	m_grid->SetDimensions(gridISize, gridJSize, 1);
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	vtkSmartPointer<vtkDoubleArray> data = vtkSmartPointer<vtkDoubleArray>::New();
	data->SetName(DATA);

	points->Initialize();
	points->Allocate(gridISize * gridJSize);
	data->Initialize();
	data->Allocate(gridISize * gridJSize);
	p = rs->headPoint()->nextPoint();
	int gridOffset = 0;

	// create grid.
	while (p != 0){
		// check condition to exit.
		if (m_abort){return false;}

		// check conditions to go out.
		if (m_restart){break;}
		if (m_canceled){break;}

		// create Grid.
		QVector2D vec;
		double height;
		vtkIdType index;

		// define data on traversal line first.

		// center point
		vec = p->position();
		height = p->lXSec()->interpolate(0).height();
		index = gridIndex(gridOffset, JDIVNUM, gridISize);
		points->InsertPoint(index, vec.x(), vec.y(), 0);
		data->InsertValue(index, height);

		// left bank
		for (int i = 0; i < JDIVNUM; ++i){
			vec = p->crosssectionPosition(p->lXSec()->interpolate((i + 1) / static_cast<double>(JDIVNUM)).position());
			height = p->lXSec()->interpolate((i + 1) / static_cast<double>(JDIVNUM)).height();
			index = gridIndex(gridOffset, JDIVNUM - i - 1, gridISize);
			points->InsertPoint(index, vec.x(), vec.y(), 0);
			data->InsertValue(index, height);
		}
		// right bank
		for (int i = 0; i < JDIVNUM; ++i){
			vec = p->crosssectionPosition(p->rXSec()->interpolate((i + 1) / static_cast<double>(JDIVNUM)).position());
			height = p->rXSec()->interpolate((i + 1) / static_cast<double>(JDIVNUM)).height();
			index = gridIndex(gridOffset, JDIVNUM + i + 1, gridISize);
			points->InsertPoint(index, vec.x(), vec.y(), 0);
			data->InsertValue(index, height);
		}

		if (p->nextPoint() != 0){
			// points on center line
			for (int i = 0; i < IDIVNUM - 1; ++i){
				vec = p->myCtrlPointPosition2D(&RawDataRiverPathPoint::riverCenter, (i + 1) / static_cast<double>(IDIVNUM));
				height = p->myHeight(RawDataRiverPathPoint::bk_LeftBank, 0, 0, (i + 1) / static_cast<double>(IDIVNUM));
				index = gridIndex(gridOffset + i + 1, JDIVNUM, gridISize);
				points->InsertPoint(index, vec.x(), vec.y(), 0);
				data->InsertValue(index, height);
			}
			// points on left bank
			for (int i = 0; i < IDIVNUM - 1; ++i){
				vec = p->myCtrlPointPosition2D(&RawDataRiverPathPoint::leftBank, (i + 1) / static_cast<double>(IDIVNUM));
				height = p->myHeight(RawDataRiverPathPoint::bk_LeftBank, 1, 1, (i + 1) / static_cast<double>(IDIVNUM));
				index = gridIndex(gridOffset + i + 1, 0, gridISize);
				points->InsertPoint(index, vec.x(), vec.y(), 0);
				data->InsertValue(index, height);
			}
			// points on right bank
			for (int i = 0; i < IDIVNUM - 1; ++i){
				vec = p->myCtrlPointPosition2D(&RawDataRiverPathPoint::rightBank, (i + 1) / static_cast<double>(IDIVNUM));
				height = p->myHeight(RawDataRiverPathPoint::bk_RightBank, 1, 1, (i + 1) / static_cast<double>(IDIVNUM));
				index = gridIndex(gridOffset + i + 1, 2 * JDIVNUM, gridISize);
				points->InsertPoint(index, vec.x(), vec.y(), 0);
				data->InsertValue(index, height);
			}
			// points between center and left.
			for (int i = 0; i < IDIVNUM - 1; ++i){
				for (int j = 0; j < JDIVNUM - 1; ++j){
					vec = p->backgroundGridCtrlPosition(RawDataRiverPathPoint::bk_LeftBank, j, (i + 1) / static_cast<double>(IDIVNUM));
					double param = (j + 1) / static_cast<double>(JDIVNUM);
					height = p->myHeight(RawDataRiverPathPoint::bk_LeftBank, param, param, (i + 1) / static_cast<double>(IDIVNUM));
					index = gridIndex(gridOffset + i + 1, JDIVNUM - j - 1, gridISize);
					points->InsertPoint(index, vec.x(), vec.y(), 0);
					data->InsertValue(index, height);
				}
			}
			// points between center and right.
			for (int i = 0; i < IDIVNUM - 1; ++i){
				for (int j = 0; j < JDIVNUM - 1; ++j){
					vec = p->backgroundGridCtrlPosition(RawDataRiverPathPoint::bk_RightBank, j, (i + 1) / static_cast<double>(IDIVNUM));
					double param = (j + 1) / static_cast<double>(JDIVNUM);
					height = p->myHeight(RawDataRiverPathPoint::bk_RightBank, param, param, (i + 1) / static_cast<double>(IDIVNUM));
					index = gridIndex(gridOffset + i + 1, JDIVNUM + j + 1, gridISize);
					points->InsertPoint(index, vec.x(), vec.y(), 0);
					data->InsertValue(index, height);
				}
			}
		}
		gridOffset += IDIVNUM;
		p = p->nextPoint();
	}
	for (auto s_it = solvers.begin(); s_it != solvers.end(); ++s_it){
		delete (*s_it);
	}
	points->Modified();
	data->Modified();
	m_grid->SetPoints(points);
	m_grid->GetPointData()->AddArray(data);
	m_grid->GetPointData()->SetActiveScalars(DATA);
	m_grid->Modified();

	// create partial grids.
	p = rs->headPoint()->nextPoint();
	gridOffset = 0;

	while (p != 0){
		if (! p->previousPoint()->firstPoint()){
			vtkSmartPointer<vtkExtractGrid> extract = vtkSmartPointer<vtkExtractGrid>::New();
			extract->SetInputData(m_grid);
			extract->SetVOI(gridOffset - IDIVNUM, gridOffset, 0, JDIVNUM * 2, 0, 0);
			extract->Update();
			vtkStructuredGrid* grid = extract->GetOutput();
			grid->Register(0);
			m_partialGrids.insert(p->previousPoint(), grid);
		}
		gridOffset += IDIVNUM;
		p = p->nextPoint();
	}
	if (! (m_abort || m_restart || m_canceled)){
		emit gridUpdated();
	}
	return true;
}

bool RawDataRiverSurveyBackgroundGridCreateThread::runUsingDivisionPoints()
{
	RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(parent());
	RawDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	RawDataRiverPathPoint* lastp = p;
	while (lastp->nextPoint() != 0){
		lastp = lastp->nextPoint();
	}
	// calculate gridsize.
	int gridISize = p->gridCounts(lastp);
	int gridJSize = p->CenterToLeftCtrlPoints.size() + p->CenterToRightCtrlPoints.size() + 3;

	// initializes grid.
	Structured2DGrid* tmpgrid = new Structured2DGrid(0);
	tmpgrid->setDimensions(gridISize, gridJSize);
	PreProcessorRawdataDataItemInterface* rawItem = dynamic_cast<PreProcessorRawdataDataItemInterface*>(rs->parent());
	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(rawItem->parent()->parent()->parent());
	SolverDefinitionGridType* gt = gtItem->gridType();
	gt->buildGridRelatedConditions(tmpgrid);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	points->InsertPoint(gridISize * gridJSize - 1, 0, 0, 0);
	tmpgrid->vtkGrid()->SetPoints(points);

	QList<GridRelatedConditionContainer*>& clist = tmpgrid->gridRelatedConditions();
	for (auto it = clist.begin(); it != clist.end(); ++it){
		(*it)->allocate();
	}
	// update grid interpolator.
	updateGridInterpolators();

	// create grid.
	unsigned int current = 0;
	while (p != lastp){
		// check conditions to go out.
		if (m_abort || m_restart || m_canceled){
			delete tmpgrid;
			return (m_abort == false);
		}

		p->createGrid(tmpgrid, current, true);
		current += static_cast<unsigned int>(p->CenterLineCtrlPoints.size() + 1);
		p = p->nextPoint();
	}
	if (m_abort || m_restart || m_canceled){
		delete tmpgrid;
		return (m_abort == false);
	}
	p->createGrid(tmpgrid, current, true, true);
	tmpgrid->setModified();

	p = rs->headPoint()->nextPoint();
	int gridOffset = 0;

	m_grid = vtkSmartPointer<vtkStructuredGrid>::New();
	m_grid->SetDimensions(gridISize, gridJSize, 1);
	m_grid->SetPoints(tmpgrid->vtkGrid()->GetPoints());
	vtkDataArray* tmpData = tmpgrid->vtkGrid()->GetPointData()->GetArray("Elevation");
	if (tmpData != 0){
		vtkSmartPointer<vtkDoubleArray> data = vtkSmartPointer<vtkDoubleArray>::New();
		data->DeepCopy(tmpData);
		data->SetName(DATA);
		m_grid->GetPointData()->AddArray(data);
		m_grid->GetPointData()->SetActiveScalars(DATA);
	}

	// create partial grids.
	while (p != 0){
		if (! p->previousPoint()->firstPoint()){
			vtkSmartPointer<vtkExtractGrid> extract = vtkSmartPointer<vtkExtractGrid>::New();
			extract->SetInputData(m_grid);
			int tmpIsize = p->previousPoint()->CenterLineCtrlPoints.size() + 1;
			extract->SetVOI(gridOffset - tmpIsize, gridOffset, 0, gridJSize, 0, 0);
			extract->Update();
			vtkStructuredGrid* pGrid = extract->GetOutput();
			pGrid->Register(0);
			m_partialGrids.insert(p->previousPoint(), pGrid);
		}
		gridOffset += p->CenterLineCtrlPoints.size() + 1;
		p = p->nextPoint();
	}
	if (! (m_abort || m_restart || m_canceled)){
		emit gridUpdated();
	}
	delete tmpgrid;
	return true;
}

void RawDataRiverSurveyBackgroundGridCreateThread::updateGridInterpolators()
{
	QList<RiverGridCtrlSolver*>::iterator it;
	// clear first.
	for (it = m_gridSolvers.begin(); it != m_gridSolvers.end(); ++it){
		delete (*it);
	}
	m_gridSolvers.clear();
	RawDataRiverSurvey* rs = dynamic_cast<RawDataRiverSurvey*>(parent());
	RawDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	// center to left
	for (int i = 0; i < p->CenterToLeftCtrlPoints.size(); ++i){
		RiverGridCtrlSolver* solver = new RiverGridCtrlSolver();
		solver->SetBankSide(RiverGridCtrlSolver::bs_LeftBank);
		solver->SetIndex(i);
		solver->setHeadPoint(rs->headPoint());
		m_gridSolvers.push_back(solver);
	}
	if (m_abort || m_restart || m_canceled){return;}

	// center to right
	for (int i = 0; i < p->CenterToRightCtrlPoints.size(); ++i){
		RiverGridCtrlSolver* solver = new RiverGridCtrlSolver();
		solver->SetBankSide(RiverGridCtrlSolver::bs_RightBank);
		solver->SetIndex(i);
		solver->setHeadPoint(rs->headPoint());
		m_gridSolvers.push_back(solver);
	}
	if (m_abort || m_restart || m_canceled){return;}

	// prepare interpolators.
	unsigned int lindices = static_cast<unsigned int>(p->CenterToLeftCtrlPoints.size());
	unsigned int rindices = static_cast<unsigned int>(p->CenterToRightCtrlPoints.size());
	QVector<Interpolator2D1*>::iterator rit;
	p = rs->headPoint();
	while (p != 0){
		for (rit = p->LGridLines().begin(); rit != p->LGridLines().end(); ++rit){
			if (*rit != 0){
				delete (*rit);
			}
		}
		for (rit = p->RGridLines().begin(); rit != p->RGridLines().end(); ++rit){
			if (*rit != 0){
				delete (*rit);
			}
		}
		p->LGridLines().clear();
		p->LGridLines().insert(0, lindices, 0);
		p->RGridLines().clear();
		p->RGridLines().insert(0, rindices, 0);
		p = p->nextPoint();
	}
	if (m_abort || m_restart || m_canceled){return;}
	for (it = m_gridSolvers.begin(); it != m_gridSolvers.end(); ++it){
		(*it)->update();
		if (m_abort || m_restart || m_canceled){return;}
	}
}

void RawDataRiverSurveyBackgroundGridCreateThread::startBGGridCopy()
{
	QMutexLocker locker(&m_mutex);
	m_copyingBGGrid = true;
}

void RawDataRiverSurveyBackgroundGridCreateThread::finishBGGridCopy()
{
	QMutexLocker locker(&m_mutex);
	m_copyingBGGrid = false;
	m_bgGridCopyFinishCondition.wakeOne();
}
