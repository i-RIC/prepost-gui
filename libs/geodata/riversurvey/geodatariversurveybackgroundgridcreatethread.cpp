#include "geodatarivercrosssection.h"
#include "geodatariverpathpoint.h"
#include "geodatarivershapeinterpolator.h"
#include "geodatariversurveybackgroundgridcreatethread.h"

#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QList>
#include <QSettings>
#include <QVector2D>

#include <vtkDoubleArray.h>
#include <vtkExtractGrid.h>
#include <vtkPointData.h>
#include <vtkPoints.h>
#include <vtkQuad.h>

#define DATA "Data"

GeoDataRiverSurveyBackgroundGridCreateThread::GeoDataRiverSurveyBackgroundGridCreateThread(GeoDataRiverSurvey* rs) :
	QThread(rs)
{
	m_useDivisionPoints = false;

	m_restart = false;
	m_abort = false;
	m_canceled = false;
	m_copyingBGGrid = false;

	m_grid = vtkSmartPointer<vtkStructuredGrid>::New();
}

GeoDataRiverSurveyBackgroundGridCreateThread::~GeoDataRiverSurveyBackgroundGridCreateThread()
{
	m_mutex.lock();
	m_abort = true;
	m_mutex.unlock();

	wait();
}

void GeoDataRiverSurveyBackgroundGridCreateThread::update()
{
	QMutexLocker locker(&m_mutex);
	if (! isRunning()) {
		start(LowestPriority);
	} else {
		m_restart = true;
		m_runCondition.wait(&m_mutex);
	}
}

void GeoDataRiverSurveyBackgroundGridCreateThread::cancel()
{
	QMutexLocker locker(&m_mutex);
	m_canceled = true;
	m_cancelCondition.wait(&m_mutex);
}

vtkIdType GeoDataRiverSurveyBackgroundGridCreateThread::gridIndex(int i, int j, int ISize)
{
	return ISize * j + i;
}

void GeoDataRiverSurveyBackgroundGridCreateThread::run()
{
	forever {
		// delete partial grids.
		m_mutex.lock();
		if (m_copyingBGGrid) {
			m_bgGridCopyFinishCondition.wait(&m_mutex);
		}
		m_mutex.unlock();
		for (auto it = m_partialGrids.begin(); it != m_partialGrids.end(); ++it) {
			vtkPointSet* tmpgrid = it.value();
			tmpgrid->Delete();
		}
		m_partialGrids.clear();

		bool ok;
		if (m_useDivisionPoints) {
			ok = runUsingDivisionPoints();
		} else {
			ok = runStandard();
		}
		if (! ok) {return;}

		m_mutex.lock();
		if (m_canceled) {
			m_canceled = false;
			m_cancelCondition.wakeOne();
		}
		m_mutex.unlock();
		while (1) {
			QMutexLocker locker(&m_mutex);
			if (m_abort) {
				return;
			}
			if (m_canceled) {
				m_canceled = false;
				m_cancelCondition.wakeOne();
			}
			if (m_restart) {
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
		for (int i = 0; i < 5 && ! m_canceled; ++i) {
			msleep(10);
		}
	}
}

bool GeoDataRiverSurveyBackgroundGridCreateThread::runStandard()
{
	QSettings settings;
	settings.beginGroup("backgroundgrid");

	int IDIVNUM = settings.value("rs_streamdirdiv", 4).value<int>();
	int JDIVNUM = settings.value("rs_csdirdiv", 20).value<int>();

	int pointCount = 0;
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(parent());
	GeoDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	while (p != nullptr) {
		++pointCount;
		p = p->nextPoint();
	}
	// calculate gridsize.
	int gridISize = pointCount + (pointCount - 1) * (IDIVNUM - 1);
	int gridJSize = JDIVNUM * 2 + 1;

	// build interpolators first.
	QList<RiverBackgroundGridCtrlSolver*> solvers;
	for (int i = 0; i < JDIVNUM - 1; ++i) {
		RiverBackgroundGridCtrlSolver* solver = new RiverBackgroundGridCtrlSolver();
		solver->setBankSide(RiverBackgroundGridCtrlSolver::bs_LeftBank);
		solver->setIndex(i);
		solver->setParameter((i + 1) / static_cast<double>(JDIVNUM));
		solver->setHeadPoint(rs->headPoint());
		solvers.append(solver);
	}
	for (int i = 0; i < JDIVNUM - 1; ++i) {
		RiverBackgroundGridCtrlSolver* solver = new RiverBackgroundGridCtrlSolver();
		solver->setBankSide(RiverBackgroundGridCtrlSolver::bs_RightBank);
		solver->setIndex(i);
		solver->setParameter((i + 1) / static_cast<double>(JDIVNUM));
		solver->setHeadPoint(rs->headPoint());
		solvers.append(solver);
	}
	// setup interpolators.
	p = rs->headPoint();
	while (p != nullptr) {
		// check condition to exit.
		if (m_abort) {return false;}

		// check conditions to go out.
		if (m_restart) {break;}
		if (m_canceled) {break;}
		if (! m_useDivisionPoints) {
			if (p->backgroundLGridLines().size() != JDIVNUM - 1) {
				for (auto line : p->backgroundLGridLines()) {
					delete line;
				}
				auto& lines = p->backgroundLGridLines();
				lines.clear();
				lines.insert(lines.begin(), JDIVNUM - 1, 0);
			}
			if (p->backgroundRGridLines().size() != JDIVNUM - 1) {
				for (auto line : p->backgroundRGridLines()) {
					delete line;
				}
				auto& lines = p->backgroundRGridLines();
				lines.clear();
				lines.insert(lines.begin(), JDIVNUM - 1, 0);
			}
		}
		p = p->nextPoint();
	}
	// build background interpolators.
	for (RiverBackgroundGridCtrlSolver* solver : solvers) {
		// check condition to exit.
		if (m_abort) {return false;}

		// check conditions to go out.
		if (m_restart) {break;}
		if (m_canceled) {break;}

		solver->update();
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
	while (p != nullptr) {
		// check condition to exit.
		if (m_abort) {return false;}

		// check conditions to go out.
		if (m_restart) {break;}
		if (m_canceled) {break;}

		// create Grid.
		QPointF vec;
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
		for (int i = 0; i < JDIVNUM; ++i) {
			vec = p->crosssectionPosition(p->lXSec()->interpolate((i + 1) / static_cast<double>(JDIVNUM)).position());
			height = p->lXSec()->interpolate((i + 1) / static_cast<double>(JDIVNUM)).height();
			index = gridIndex(gridOffset, JDIVNUM - i - 1, gridISize);
			points->InsertPoint(index, vec.x(), vec.y(), 0);
			data->InsertValue(index, height);
		}
		// right bank
		for (int i = 0; i < JDIVNUM; ++i) {
			vec = p->crosssectionPosition(p->rXSec()->interpolate((i + 1) / static_cast<double>(JDIVNUM)).position());
			height = p->rXSec()->interpolate((i + 1) / static_cast<double>(JDIVNUM)).height();
			index = gridIndex(gridOffset, JDIVNUM + i + 1, gridISize);
			points->InsertPoint(index, vec.x(), vec.y(), 0);
			data->InsertValue(index, height);
		}

		if (p->nextPoint() != nullptr) {
			// points on center line
			for (int i = 0; i < IDIVNUM - 1; ++i) {
				vec = p->myCtrlPointPosition2D(&GeoDataRiverPathPoint::riverCenter, (i + 1) / static_cast<double>(IDIVNUM));
				height = p->myHeight(GeoDataRiverPathPoint::bk_LeftBank, 0, 0, (i + 1) / static_cast<double>(IDIVNUM));
				index = gridIndex(gridOffset + i + 1, JDIVNUM, gridISize);
				points->InsertPoint(index, vec.x(), vec.y(), 0);
				data->InsertValue(index, height);
			}
			// points on left bank
			for (int i = 0; i < IDIVNUM - 1; ++i) {
				vec = p->myCtrlPointPosition2D(&GeoDataRiverPathPoint::leftBank, (i + 1) / static_cast<double>(IDIVNUM));
				height = p->myHeight(GeoDataRiverPathPoint::bk_LeftBank, 1, 1, (i + 1) / static_cast<double>(IDIVNUM));
				index = gridIndex(gridOffset + i + 1, 0, gridISize);
				points->InsertPoint(index, vec.x(), vec.y(), 0);
				data->InsertValue(index, height);
			}
			// points on right bank
			for (int i = 0; i < IDIVNUM - 1; ++i) {
				vec = p->myCtrlPointPosition2D(&GeoDataRiverPathPoint::rightBank, (i + 1) / static_cast<double>(IDIVNUM));
				height = p->myHeight(GeoDataRiverPathPoint::bk_RightBank, 1, 1, (i + 1) / static_cast<double>(IDIVNUM));
				index = gridIndex(gridOffset + i + 1, 2 * JDIVNUM, gridISize);
				points->InsertPoint(index, vec.x(), vec.y(), 0);
				data->InsertValue(index, height);
			}
			// points between center and left.
			for (int i = 0; i < IDIVNUM - 1; ++i) {
				for (int j = 0; j < JDIVNUM - 1; ++j) {
					vec = p->backgroundGridCtrlPosition(GeoDataRiverPathPoint::bk_LeftBank, j, (i + 1) / static_cast<double>(IDIVNUM));
					double param = (j + 1) / static_cast<double>(JDIVNUM);
					height = p->myHeight(GeoDataRiverPathPoint::bk_LeftBank, param, param, (i + 1) / static_cast<double>(IDIVNUM));
					index = gridIndex(gridOffset + i + 1, JDIVNUM - j - 1, gridISize);
					points->InsertPoint(index, vec.x(), vec.y(), 0);
					data->InsertValue(index, height);
				}
			}
			// points between center and right.
			for (int i = 0; i < IDIVNUM - 1; ++i) {
				for (int j = 0; j < JDIVNUM - 1; ++j) {
					vec = p->backgroundGridCtrlPosition(GeoDataRiverPathPoint::bk_RightBank, j, (i + 1) / static_cast<double>(IDIVNUM));
					double param = (j + 1) / static_cast<double>(JDIVNUM);
					height = p->myHeight(GeoDataRiverPathPoint::bk_RightBank, param, param, (i + 1) / static_cast<double>(IDIVNUM));
					index = gridIndex(gridOffset + i + 1, JDIVNUM + j + 1, gridISize);
					points->InsertPoint(index, vec.x(), vec.y(), 0);
					data->InsertValue(index, height);
				}
			}
		}
		gridOffset += IDIVNUM;
		p = p->nextPoint();
	}
	for (auto s_it = solvers.begin(); s_it != solvers.end(); ++s_it) {
		delete(*s_it);
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

	while (p != nullptr) {
		if (! p->previousPoint()->firstPoint()) {
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
	if (!(m_abort || m_restart || m_canceled)) {
		emit gridUpdated();
	}
	return true;
}

bool GeoDataRiverSurveyBackgroundGridCreateThread::runUsingDivisionPoints()
{
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(parent());
	GeoDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	GeoDataRiverPathPoint* lastp = p;
	while (lastp->nextPoint() != nullptr) {
		lastp = lastp->nextPoint();
	}
	// calculate gridsize.
	int gridISize = p->gridCounts(lastp);
	int gridJSize = p->CenterToLeftCtrlPoints.size() + p->CenterToRightCtrlPoints.size() + 3;

	// initializes grid.
	Structured2DGrid* tmpgrid = new Structured2DGrid(nullptr);
	tmpgrid->setDimensions(gridISize, gridJSize);
	PreProcessorGeoDataDataItemInterface* rawItem = dynamic_cast<PreProcessorGeoDataDataItemInterface*>(rs->parent());
	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(rawItem->parent()->parent()->parent());
	SolverDefinitionGridType* gt = gtItem->gridType();
	gt->buildGridAttributes(tmpgrid);

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	points->InsertPoint(gridISize * gridJSize - 1, 0, 0, 0);
	tmpgrid->vtkGrid()->SetPoints(points);

	QList<GridAttributeContainer*>& clist = tmpgrid->gridAttributes();
	for (auto it = clist.begin(); it != clist.end(); ++it) {
		(*it)->allocate();
	}
	// update grid interpolator.
	updateGridInterpolators();

	// create grid.
	unsigned int current = 0;
	while (p != lastp) {
		// check conditions to go out.
		if (m_abort || m_restart || m_canceled) {
			delete tmpgrid;
			return (m_abort == false);
		}

		p->createGrid(tmpgrid, current, true);
		current += static_cast<unsigned int>(p->CenterLineCtrlPoints.size() + 1);
		p = p->nextPoint();
	}
	if (m_abort || m_restart || m_canceled) {
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
	if (tmpData != nullptr) {
		vtkSmartPointer<vtkDoubleArray> data = vtkSmartPointer<vtkDoubleArray>::New();
		data->DeepCopy(tmpData);
		data->SetName(DATA);
		m_grid->GetPointData()->AddArray(data);
		m_grid->GetPointData()->SetActiveScalars(DATA);
	}

	// create partial grids.
	while (p != nullptr) {
		if (! p->previousPoint()->firstPoint()) {
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
	if (!(m_abort || m_restart || m_canceled)) {
		emit gridUpdated();
	}
	delete tmpgrid;
	return true;
}

void GeoDataRiverSurveyBackgroundGridCreateThread::updateGridInterpolators()
{
	// clear first.
	for (auto solver : m_gridSolvers) {
		delete solver;
	}
	m_gridSolvers.clear();
	GeoDataRiverSurvey* rs = dynamic_cast<GeoDataRiverSurvey*>(parent());
	GeoDataRiverPathPoint* p = rs->headPoint()->nextPoint();
	// center to left
	for (int i = 0; i < p->CenterToLeftCtrlPoints.size(); ++i) {
		RiverGridCtrlSolver* solver = new RiverGridCtrlSolver();
		solver->setBankSide(RiverGridCtrlSolver::bs_LeftBank);
		solver->setIndex(i);
		solver->setHeadPoint(rs->headPoint());
		m_gridSolvers.push_back(solver);
	}
	if (m_abort || m_restart || m_canceled) {return;}

	// center to right
	for (int i = 0; i < p->CenterToRightCtrlPoints.size(); ++i) {
		RiverGridCtrlSolver* solver = new RiverGridCtrlSolver();
		solver->setBankSide(RiverGridCtrlSolver::bs_RightBank);
		solver->setIndex(i);
		solver->setHeadPoint(rs->headPoint());
		m_gridSolvers.push_back(solver);
	}
	if (m_abort || m_restart || m_canceled) {return;}

	// prepare interpolators.
	unsigned int lindices = static_cast<unsigned int>(p->CenterToLeftCtrlPoints.size());
	unsigned int rindices = static_cast<unsigned int>(p->CenterToRightCtrlPoints.size());
	p = rs->headPoint();
	while (p != nullptr) {
		for (auto line : p->LGridLines()) {
			delete line;
		}
		for (auto line : p->RGridLines()) {
			delete line;
		}
		auto& llines = p->LGridLines();
		llines.clear();
		llines.insert(llines.begin(), lindices, 0);
		auto& rlines = p->RGridLines();
		rlines.clear();
		rlines.insert(rlines.begin(), rindices, 0);
		p = p->nextPoint();
	}
	if (m_abort || m_restart || m_canceled) {return;}
	for (auto it = m_gridSolvers.begin(); it != m_gridSolvers.end(); ++it) {
		(*it)->update();
		if (m_abort || m_restart || m_canceled) {return;}
	}
}

void GeoDataRiverSurveyBackgroundGridCreateThread::startBGGridCopy()
{
	QMutexLocker locker(&m_mutex);
	m_copyingBGGrid = true;
}

void GeoDataRiverSurveyBackgroundGridCreateThread::finishBGGridCopy()
{
	QMutexLocker locker(&m_mutex);
	m_copyingBGGrid = false;
	m_bgGridCopyFinishCondition.wakeOne();
}
