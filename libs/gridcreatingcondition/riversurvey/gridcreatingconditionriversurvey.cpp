#include "gridcreatingconditionriversurvey.h"
#include "gridcreatingconditionriversurveypointadddialog.h"
#include "gridcreatingconditionriversurveypointmovedialog.h"
#include "gridcreatingconditionriversurveypointregionadddialog.h"
#include "gridcreatingconditionriversurveypointrepositiondialog.h"
#include "gridcreatingconditionriversurveyregiondialog.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatadataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorrawdatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>
#include <misc/zdepthrange.h>
#include <rawdata/riversurvey/rawdatariversurvey.h>
#include <rawdata/riversurvey/rawdatariversurveyctrlpointbackup.h>

#include <QInputEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QToolBar>
#include <QUndoCommand>
#include <QXmlStreamWriter>

#include <vtkExtractGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkVertex.h>

#include <cmath>

class GridCreatingConditionCtrlPointDeleteCommand : public QUndoCommand
{
public:
	GridCreatingConditionCtrlPointDeleteCommand(GridCreatingConditionRiverSurvey* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey::tr("Delete Selected Control Points")) {
		m_condition = cond;
		executeDeleteCtrlPoints();
	}

	~GridCreatingConditionCtrlPointDeleteCommand()
	{}

	void undo() {
		m_condition->cancelBackgroundGridUpdate();
		for (RawDataRiverSurveyCtrlPointBackup* backup : m_before) {
			backup->restore();
		}

		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
		m_condition->informCtrlPointUpdateToCrosssectionWindows();
	}

	void redo() {
		m_condition->cancelBackgroundGridUpdate();
		for (RawDataRiverSurveyCtrlPointBackup* backup : m_after) {
			backup->restore();
		}

		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
		m_condition->informCtrlPointUpdateToCrosssectionWindows();
	}

private:
	void executeDeleteCtrlPoints() {
		RawDataRiverSurveyCtrlPointBackup* backup;

		typedef std::set<int> iset;
		iset EmptyIndices;
		iset LeftRemoveIndices;
		iset RightRemoveIndices;

		std::set<RawDataRiverPathPoint*> points;
		std::map<RawDataRiverPathPoint*, iset> PointIndices;

		bool RemoveCenterToLeft = false;
		bool RemoveCenterToRight = false;

		for (auto it = m_condition->m_selectedCtrlPointInfoList.begin(); it != m_condition->m_selectedCtrlPointInfoList.end(); ++it) {
			if (it->Position == RawDataRiverPathPoint::pposCenterToLeft) {
				RemoveCenterToLeft = true;
				LeftRemoveIndices.insert(it->Index);
			} else if (it->Position == RawDataRiverPathPoint::pposCenterToRight) {
				RemoveCenterToRight = true;
				RightRemoveIndices.insert(it->Index);
			} else {
				points.insert(it->Point);
				auto psetit = PointIndices.find(it->Point);
				if (psetit == PointIndices.end()) {
					std::pair<RawDataRiverPathPoint*, iset> pi_pair =
						std::pair<RawDataRiverPathPoint*, iset>(it->Point, EmptyIndices);
					auto insertresult = PointIndices.insert(pi_pair);
					psetit = insertresult.first;
				}
				psetit->second.insert(it->Index);
			}
		}

		RawDataRiverPathPoint* headPoint = m_condition->m_riverSurvey->headPoint();

		// Create Backup.
		if (RemoveCenterToLeft) {
			backup = new RawDataRiverSurveyCtrlPointBackup();
			backup->backup(headPoint, RawDataRiverPathPoint::zposCenterToLeft);
			m_before.push_back(backup);
		}
		if (RemoveCenterToRight) {
			backup = new RawDataRiverSurveyCtrlPointBackup();
			backup->backup(headPoint, RawDataRiverPathPoint::zposCenterToRight);
			m_before.push_back(backup);
		}
		for (RawDataRiverPathPoint* point : points) {
			backup = new RawDataRiverSurveyCtrlPointBackup();
			backup->backup(point, RawDataRiverPathPoint::zposCenterLine);
			m_before.push_back(backup);
		}

		// Execute Deletion.
		if (RemoveCenterToLeft) {
			RawDataRiverPathPoint* tmpp = m_condition->m_riverSurvey->headPoint();
			if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
			while (tmpp != nullptr) {
				tmpp->removeCtrlPoints(RawDataRiverPathPoint::zposCenterToLeft, LeftRemoveIndices);
				tmpp = tmpp->nextPoint();
			}
		}
		if (RemoveCenterToRight) {
			RawDataRiverPathPoint* tmpp = m_condition->m_riverSurvey->headPoint();
			if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
			while (tmpp != nullptr) {
				tmpp->removeCtrlPoints(RawDataRiverPathPoint::zposCenterToRight, RightRemoveIndices);
				tmpp = tmpp->nextPoint();
			}
		}
		for (auto psetit = PointIndices.begin(); psetit != PointIndices.end(); ++psetit) {
			(psetit->first)->removeCtrlPoints(RawDataRiverPathPoint::zposCenterLine, psetit->second);
		}

		// Create Backup
		if (RemoveCenterToLeft) {
			backup = new RawDataRiverSurveyCtrlPointBackup();
			backup->backup(headPoint, RawDataRiverPathPoint::zposCenterToLeft);
			m_after.push_back(backup);
		}
		if (RemoveCenterToRight) {
			backup = new RawDataRiverSurveyCtrlPointBackup();
			backup->backup(headPoint, RawDataRiverPathPoint::zposCenterToRight);
			m_after.push_back(backup);
		}
		for (RawDataRiverPathPoint* point : points) {
			backup = new RawDataRiverSurveyCtrlPointBackup();
			backup->backup(point, RawDataRiverPathPoint::zposCenterLine);
			m_after.push_back(backup);
		}

		m_condition->m_selectedCtrlPointInfoList.clear();
		m_condition->m_analogyCtrlPointInfoList.clear();
	}

private:
	GridCreatingConditionRiverSurvey* m_condition;
	std::list<RawDataRiverSurveyCtrlPointBackup*> m_before;
	std::list<RawDataRiverSurveyCtrlPointBackup*> m_after;
};


// constructor
GridCreatingConditionRiverSurvey::GridCreatingConditionRiverSurvey(ProjectDataItem* parent, GridCreatingConditionCreator* creator)
	: GridCreatingConditionRiverSurveyInterface(parent, creator)
{
	m_riverSurvey = nullptr;
	m_lastStartPoint = nullptr;
	m_lastEndPoint = nullptr;

	m_lastRegionAddStartPoint = nullptr;
	m_lastRegionAddEndPoint = nullptr;

	m_selectedZone.point = nullptr;

	m_mouseEventMode = meNormal;
	m_rightClickingMenu = nullptr;
	setupVtkContainers();
	setupActions();
	setActionStatus();
}

GridCreatingConditionRiverSurvey::~GridCreatingConditionRiverSurvey()
{
	vtkRenderer* r = renderer();

	r->RemoveActor(m_leftBankLineActor);
	r->RemoveActor(m_rightBankLineActor);
	r->RemoveActor(m_riverCenterLineActor);
	r->RemoveActor(m_crossSectionsActor);
	r->RemoveActor(m_fixedPointsActor);
	r->RemoveActor(m_blackFixedPointsActor);
	r->RemoveActor(m_ctrlPointsActor);
	r->RemoveActor(m_blackCtrlPointsActor);
	r->RemoveActor(m_selectedCtrlZoneActor);
	r->RemoveActor(m_createRegionActor);

	r->RemoveActor(m_selectedCtrlPointsActor);
	r->RemoveActor(m_blackSelectedCtrlPointsActor);
	r->RemoveActor(m_analogyCtrlPointsActor);
	r->RemoveActor(m_blackAnalogyCtrlPointsActor);

	delete m_rightClickingMenu;
	if (m_riverSurvey != nullptr) {
		m_riverSurvey->useDivisionPointsForBackgroundGrid(false);
	}
}

bool GridCreatingConditionRiverSurvey::create(QWidget* /*parent*/)
{
	GridCreatingConditionRiverSurveyRegionDialog* dialog = new GridCreatingConditionRiverSurveyRegionDialog(this, preProcessorWindow());
	dialog->setData(m_riverSurvey);
	dialog->setStartPoint(m_lastStartPoint);
	dialog->setEndPoint(m_lastEndPoint);
	dialog->update();
	m_mouseEventMode = meCreationDialog;
	iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(hideCreateRegion()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));

	m_createRegionActor->VisibilityOn();
	dialog->show();
	return false;
}

bool GridCreatingConditionRiverSurvey::ready() const
{
	return true;
}

bool GridCreatingConditionRiverSurvey::init()
{
	// set m_riverSurvey.
	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(parent()->parent()->parent());
	PreProcessorRawDataTopDataItemInterface* rtItem = gtItem->rawdataTop();
	bool found = false;

	for (PreProcessorRawDataGroupDataItemInterface* gItem : rtItem->groupDataItems()) {
		QList<PreProcessorRawdataDataItemInterface*> rItems = gItem->rawDatas();
		for (auto rit = rItems.begin(); ! found && rit != rItems.end(); ++rit) {
			PreProcessorRawdataDataItemInterface* rItem = *rit;
			if (dynamic_cast<RawDataRiverSurvey*>(rItem->rawData()) != nullptr) {
				// this is a river survey data!

				// @todo currently, the river survey data found first, is
				// automatically selected.
				// For cases when there are multiple river survey data, this
				// implementation should be changed.
				m_riverSurvey = dynamic_cast<RawDataRiverSurvey*>(rItem->rawData());
				m_riverSurvey->setGridCreatingCondition(this);
				m_riverSurvey->useDivisionPointsForBackgroundGrid(true);
				connect(m_riverSurvey, SIGNAL(dataUpdated()), this, SLOT(handleDataChange()));
				connect(m_riverSurvey, SIGNAL(destroyed()), this, SLOT(handleDataDestroy()));
				found = true;
			}
		}
	}
	if (! found) {
		// there is no river survey data.
		QMessageBox::warning(dataModel()->mainWindow(), tr("Error"), tr("There is no river survey data. This grid creating condition is not available."));
		return false;
	}
	updateShapeData();

	RawDataRiverPathPoint* pp = m_riverSurvey->headPoint();
	while (pp != nullptr) {
		if (pp->name() == m_lastStartPointName) {m_lastStartPoint = pp;}
		if (pp->name() == m_lastEndPointName) {m_lastEndPoint = pp;}
		pp = pp->nextPoint();
	}

	return true;
}

void GridCreatingConditionRiverSurvey::setupVtkContainers()
{
	// setup points.
	m_crossSectionPoints = vtkSmartPointer<vtkPoints>::New();           // points for cross section data.
	m_crossSectionPoints->SetDataTypeToDouble();
	m_ctrlPointPoints = vtkSmartPointer<vtkPoints>::New();              // control points set for grid generation.
	m_ctrlPointPoints->SetDataTypeToDouble();
	m_selectedCtrlPointPoints = vtkSmartPointer<vtkPoints>::New();      // selected control points
	m_selectedCtrlPointPoints->SetDataTypeToDouble();
	m_analogyCtrlPointPoints = vtkSmartPointer<vtkPoints>::New();       // points relative to selected control points.
	m_analogyCtrlPointPoints->SetDataTypeToDouble();

	// setup grid. (for graphic data)
	m_leftBankLine = vtkSmartPointer<vtkStructuredGrid>::New();
	m_rightBankLine = vtkSmartPointer<vtkStructuredGrid>::New();
	m_riverCenterLine = vtkSmartPointer<vtkStructuredGrid>::New();

	m_crossSections = vtkSmartPointer<vtkUnstructuredGrid>::New();          // lines which consist of cross sections.
	m_fixedPoints = vtkSmartPointer<vtkUnstructuredGrid>::New();            // points which consist of river survey lines.
	m_ctrlPoints = vtkSmartPointer<vtkUnstructuredGrid>::New();             // control points.
	m_selectedCtrlPoints = vtkSmartPointer<vtkUnstructuredGrid>::New();     // selected control points
	m_analogyCtrlPoints = vtkSmartPointer<vtkUnstructuredGrid>::New();      // points relative to selected control points.
	m_selectedCtrlZone = vtkSmartPointer<vtkStructuredGrid>::New();         // selected zone

	m_createRegion = vtkSmartPointer<vtkAppendPolyData>::New();
}

void GridCreatingConditionRiverSurvey::setupActors()
{
	vtkSmartPointer<vtkDataSetMapper> mapper;
	vtkRenderer* r = renderer();

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_crossSections);

	m_crossSectionsActor = vtkSmartPointer<vtkActor>::New();
	m_crossSectionsActor->SetMapper(mapper);
	m_crossSectionsActor->GetProperty()->SetColor(0, 0, 0);     // black.
	r->AddActor(m_crossSectionsActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_fixedPoints);

	m_fixedPointsActor = vtkSmartPointer<vtkActor>::New();
	m_fixedPointsActor->SetMapper(mapper);
	m_fixedPointsActor->GetProperty()->SetLighting(false);
	m_fixedPointsActor->GetProperty()->SetColor(0, 0, 1);       // blue.
	m_fixedPointsActor->GetProperty()->SetPointSize(3);
	r->AddActor(m_fixedPointsActor);

	m_blackFixedPointsActor = vtkSmartPointer<vtkActor>::New();
	m_blackFixedPointsActor->SetMapper(mapper);
	m_blackFixedPointsActor->GetProperty()->SetLighting(false);
	m_blackFixedPointsActor->GetProperty()->SetColor(0, 0, 0);
	m_blackFixedPointsActor->GetProperty()->SetPointSize(5);
	r->AddActor(m_blackFixedPointsActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_ctrlPoints);

	m_ctrlPointsActor = vtkSmartPointer<vtkActor>::New();
	m_ctrlPointsActor->SetMapper(mapper);
	m_ctrlPointsActor->GetProperty()->SetLighting(false);
	m_ctrlPointsActor->GetProperty()->SetColor(1, 1, 0);        // yellow.
	m_ctrlPointsActor->GetProperty()->SetPointSize(3);
	r->AddActor(m_ctrlPointsActor);

	m_blackCtrlPointsActor = vtkSmartPointer<vtkActor>::New();
	m_blackCtrlPointsActor->SetMapper(mapper);
	m_blackCtrlPointsActor->GetProperty()->SetLighting(false);
	m_blackCtrlPointsActor->GetProperty()->SetColor(0, 0, 0);
	m_blackCtrlPointsActor->GetProperty()->SetPointSize(5);
	r->AddActor(m_blackCtrlPointsActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_selectedCtrlPoints);

	m_selectedCtrlPointsActor = vtkSmartPointer<vtkActor>::New();
	m_selectedCtrlPointsActor->SetMapper(mapper);
	m_selectedCtrlPointsActor->GetProperty()->SetColor(1, 1, 0);            // yellow.
	m_selectedCtrlPointsActor->GetProperty()->SetPointSize(7);              // selected division point size is 7 pixel.
	r->AddActor(m_selectedCtrlPointsActor);

	m_blackSelectedCtrlPointsActor = vtkSmartPointer<vtkActor>::New();
	m_blackSelectedCtrlPointsActor->SetMapper(mapper);
	m_blackSelectedCtrlPointsActor->GetProperty()->SetColor(0, 0, 0);       // black.
	m_blackSelectedCtrlPointsActor->GetProperty()->SetPointSize(9);         // selected division point size is 9 pixel.
	r->AddActor(m_blackSelectedCtrlPointsActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_analogyCtrlPoints);

	m_analogyCtrlPointsActor = vtkSmartPointer<vtkActor>::New();
	m_analogyCtrlPointsActor->SetMapper(mapper);
	m_analogyCtrlPointsActor->GetProperty()->SetColor(1, 0.25, 0);          // brown
	m_analogyCtrlPointsActor->GetProperty()->SetPointSize(7);               // analogy division point size is 7 pixel.
	r->AddActor(m_analogyCtrlPointsActor);

	m_blackAnalogyCtrlPointsActor = vtkSmartPointer<vtkActor>::New();
	m_blackAnalogyCtrlPointsActor->SetMapper(mapper);
	m_blackAnalogyCtrlPointsActor->GetProperty()->SetColor(0, 0, 0);        // black.
	m_blackAnalogyCtrlPointsActor->GetProperty()->SetPointSize(9);          // selected division point size is 9 pixel.
	r->AddActor(m_blackAnalogyCtrlPointsActor);

	vtkSmartPointer<vtkStructuredGridGeometryFilter> f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	f->SetInputData(m_riverCenterLine);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(f->GetOutputPort());

	m_riverCenterLineActor = vtkSmartPointer<vtkLODActor>::New();
	m_riverCenterLineActor->SetMapper(mapper);

	// register grid with lower resolution.
	vtkSmartPointer<vtkStructuredGrid> tmpgrid = m_riverCenterLine;
	for (int i = 0; i < 2; ++i) {
		vtkSmartPointer<vtkExtractGrid> ext = vtkSmartPointer<vtkExtractGrid>::New();
		ext->SetInputData(tmpgrid);
		ext->SetSampleRate(3, 1, 1);
		f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		f->SetInputConnection(ext->GetOutputPort());

		mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		mapper->SetInputConnection(f->GetOutputPort());
		m_riverCenterLineActor->AddLODMapper(mapper);
		tmpgrid = ext->GetOutput();
	}

	m_riverCenterLineActor->GetProperty()->SetLighting(false);
	m_riverCenterLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_riverCenterLineActor);

	// Left bank line
	f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	f->SetInputData(m_leftBankLine);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(f->GetOutputPort());

	m_leftBankLineActor = vtkSmartPointer<vtkLODActor>::New();
	m_leftBankLineActor->SetMapper(mapper);

	// register grid with lower resolution.
	tmpgrid = m_leftBankLine;
	for (int i = 0; i < 2; ++i) {
		vtkSmartPointer<vtkExtractGrid> ext = vtkSmartPointer<vtkExtractGrid>::New();
		ext->SetInputData(tmpgrid);
		ext->SetSampleRate(3, 1, 1);
		f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		f->SetInputConnection(ext->GetOutputPort());

		mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		mapper->SetInputConnection(f->GetOutputPort());
		m_leftBankLineActor->AddLODMapper(mapper);
		tmpgrid = ext->GetOutput();
	}

	m_leftBankLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_leftBankLineActor);
	m_leftBankLineActor->GetProperty()->SetLighting(false);
	m_leftBankLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_leftBankLineActor);

	// Right bank line
	f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	f->SetInputData(m_rightBankLine);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(f->GetOutputPort());

	m_rightBankLineActor = vtkSmartPointer<vtkLODActor>::New();
	m_rightBankLineActor->SetMapper(mapper);

	// register grid with lower resolution.
	tmpgrid = m_rightBankLine;
	for (int i = 0; i < 2; ++i) {
		vtkSmartPointer<vtkExtractGrid> ext = vtkSmartPointer<vtkExtractGrid>::New();
		ext->SetInputData(tmpgrid);
		ext->SetSampleRate(3, 1, 1);
		f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
		f->SetInputConnection(ext->GetOutputPort());

		mapper = vtkSmartPointer<vtkDataSetMapper>::New();
		mapper->SetInputConnection(f->GetOutputPort());
		m_rightBankLineActor->AddLODMapper(mapper);
		tmpgrid = ext->GetOutput();
	}

	m_rightBankLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_rightBankLineActor);
	m_rightBankLineActor->GetProperty()->SetLighting(false);
	m_rightBankLineActor->GetProperty()->SetColor(0, 0, 0);
	r->AddActor(m_rightBankLineActor);

	f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	f->SetInputData(m_selectedCtrlZone);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(f->GetOutputPort());
	mapper->SetScalarVisibility(0);

	m_selectedCtrlZoneActor = vtkSmartPointer<vtkActor>::New();
	m_selectedCtrlZoneActor->SetMapper(mapper);
	m_selectedCtrlZoneActor->GetProperty()->SetColor(0, 0, 1.);
	m_selectedCtrlZoneActor->GetProperty()->SetLineWidth(5);
	m_selectedCtrlZoneActor->GetProperty()->SetOpacity(.5);
	m_selectedCtrlZoneActor->GetProperty()->SetLighting(false);
	r->AddActor(m_selectedCtrlZoneActor);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(m_createRegion->GetOutputPort());
	mapper->SetScalarVisibility(0);

	m_createRegionActor = vtkSmartPointer<vtkActor>::New();
	m_createRegionActor->SetMapper(mapper);
	m_createRegionActor->GetProperty()->SetColor(0, 0, 1.);
	m_createRegionActor->GetProperty()->SetOpacity(.5);
	m_createRegionActor->GetProperty()->SetLighting(false);
	m_createRegionActor->VisibilityOff();
	r->AddActor(m_createRegionActor);

	actorCollection()->AddItem(m_crossSectionsActor);
	actorCollection()->AddItem(m_fixedPointsActor);
	actorCollection()->AddItem(m_ctrlPointsActor);
	actorCollection()->AddItem(m_selectedCtrlPointsActor);
	actorCollection()->AddItem(m_analogyCtrlPointsActor);
	actorCollection()->AddItem(m_blackFixedPointsActor);
	actorCollection()->AddItem(m_blackCtrlPointsActor);
	actorCollection()->AddItem(m_blackSelectedCtrlPointsActor);
	actorCollection()->AddItem(m_blackAnalogyCtrlPointsActor);
	actorCollection()->AddItem(m_riverCenterLineActor);
	actorCollection()->AddItem(m_leftBankLineActor);
	actorCollection()->AddItem(m_rightBankLineActor);
	actorCollection()->AddItem(m_selectedCtrlZoneActor);
}

void GridCreatingConditionRiverSurvey::setupMenu()
{
	// Menu
	m_menu->addAction(m_addAction);
	m_menu->addAction(m_addRegionallyAction);
	m_menu->addSeparator();
	m_menu->addAction(m_deleteAction);
	m_menu->addSeparator();
	m_menu->addAction(m_moveAction);
	m_menu->addAction(m_repositionAction);
	m_menu->addSeparator();

	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	// ContextMenu
	if (m_rightClickingMenu == nullptr) {
		m_rightClickingMenu = new QMenu();
		m_rightClickingMenu->addAction(p->createAction());
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_addAction);
		m_rightClickingMenu->addAction(m_addRegionallyAction);
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_deleteAction);
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_moveAction);
		m_rightClickingMenu->addAction(m_repositionAction);
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(p->clearAction());
	}
}

void GridCreatingConditionRiverSurvey::informSelection(PreProcessorGraphicsViewInterface* /*v*/)
{
	updateShapeData(true);
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	col->RemoveAllItems();

	col->AddItem(m_crossSectionsActor);
	col->AddItem(m_fixedPointsActor);
	col->AddItem(m_ctrlPointsActor);
	col->AddItem(m_selectedCtrlPointsActor);
	col->AddItem(m_analogyCtrlPointsActor);
	col->AddItem(m_blackFixedPointsActor);
	col->AddItem(m_blackCtrlPointsActor);
	col->AddItem(m_blackSelectedCtrlPointsActor);
	col->AddItem(m_blackAnalogyCtrlPointsActor);
	col->AddItem(m_riverCenterLineActor);
	col->AddItem(m_leftBankLineActor);
	col->AddItem(m_rightBankLineActor);
	col->AddItem(m_selectedCtrlZoneActor);

	updateVisibility();
	if (m_riverSurvey != nullptr) {
		m_riverSurvey->toggleCrosssectionWindowsGridCreatingMode(true);
	}
}

void GridCreatingConditionRiverSurvey::informDeselection(PreProcessorGraphicsViewInterface* /*v*/)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	col->RemoveAllItems();

	col->AddItem(m_crossSectionsActor);
	col->AddItem(m_fixedPointsActor);
	col->AddItem(m_ctrlPointsActor);
	col->AddItem(m_selectedCtrlPointsActor);
	col->AddItem(m_blackFixedPointsActor);
	col->AddItem(m_blackCtrlPointsActor);
	col->AddItem(m_blackSelectedCtrlPointsActor);
	col->AddItem(m_riverCenterLineActor);
	col->AddItem(m_leftBankLineActor);
	col->AddItem(m_rightBankLineActor);
	col->AddItem(m_selectedCtrlZoneActor);

	updateVisibility();
	if (m_riverSurvey != nullptr) {
		m_riverSurvey->toggleCrosssectionWindowsGridCreatingMode(false);
	}
}

void GridCreatingConditionRiverSurvey::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GridCreatingConditionRiverSurvey::keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionRiverSurvey::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionRiverSurvey::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionRiverSurvey::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	double worldX = (double) event->x();
	double worldY = (double) event->y();
	v->viewportToWorld(worldX, worldY);
	QVector2D pvec(worldX, worldY);
	double stdSize = v->stdRadius(1);

	if (m_mouseEventMode == meNormal) {
		// drawing bounding box using mouse dragging.
		MouseBoundingBox* box = dataModel()->mouseBoundingBox();
		box->setEndPoint(event->x(), event->y());
		renderGraphicsView();
	}
	if ((m_mouseEventMode == meNormal || m_mouseEventMode == meMovePrepare) &&
			isCtrlPointsContinuousSelection()) {
		m_mouseEventMode = meNormal;
		// check whether the mouse pointer is near to one of the selected points
		for (auto it = m_selectedCtrlPointInfoList.begin(); it != m_selectedCtrlPointInfoList.end(); ++it) {
			CtrlPointSelectionInfo info = *it;
			try {
				QVector2D tmpv = info.Point->CtrlPointPosition2D(info.Position, info.Index);
				if ((pvec - tmpv).lengthSquared() < stdSize * stdSize * 9) {
					// near enough.
					m_mouseEventMode = meMovePrepare;
					updateMouseCursor(v);
					return;
				}
			} catch (RawDataRiverPathPoint::ErrorCodes) {
				// ignore the exception.
			}
		}
		updateMouseCursor(v);
	} else if (m_mouseEventMode == meMove) {
		double startX = m_dragStartPoint.x();
		double startY = m_dragStartPoint.y();
		v->viewportToWorld(startX, startY);
		QVector2D startP(startX, startY);
		QVector2D delta = pvec - startP;
		double tmpdbl = QVector2D::dotProduct(delta, m_GCPOffsetInfo.direction) / m_GCPOffsetInfo.length;
		if (tmpdbl < m_GCPOffsetInfo.lowerLimit) {tmpdbl = m_GCPOffsetInfo.lowerLimit;}
		if (tmpdbl > m_GCPOffsetInfo.upperLimit) {tmpdbl = m_GCPOffsetInfo.upperLimit;}
		iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointMoveCommand(false, tmpdbl, this));
	}
}

void GridCreatingConditionRiverSurvey::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		if (m_mouseEventMode == meNormal) {
			// Bounding box for selection started.
			MouseBoundingBox* box = dataModel()->mouseBoundingBox();
			box->setStartPoint(event->x(), event->y());
			box->enable();
			renderGraphicsView();
		} else if (m_mouseEventMode == meMovePrepare) {
			m_mouseEventMode = meMove;
			updateMouseCursor(v);
		}
	}
	m_dragStartPoint = event->pos();
}

void GridCreatingConditionRiverSurvey::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{
	if (event->button() == Qt::LeftButton) {
		if (m_mouseEventMode == meNormal) {

			// Bounding box for selection ended.
			MouseBoundingBox* box = dataModel()->mouseBoundingBox();
			box->setEndPoint(event->x(), event->y());

			// Small selection area is modified here.
			QPoint diffVec = box->endPoint() - box->startPoint();
			QPoint sP = box->startPoint();
			QPoint eP = box->endPoint();
			if (- 2 < diffVec.x() && diffVec.x() < 2 && - 2 < diffVec.y() && diffVec.y() < 2) {
				sP = sP + QPoint(- 4, - 4);
				eP = eP + QPoint(4, 4);
			}

			//
			// Points related to selected rectangular area.
			//    s
			// p0 *---------* p1
			//    |         |
			//    |         |
			// p2 *---------* e
			//
			double sX = sP.x();
			double sY = sP.y();
			double eX = eP.x();
			double eY = eP.y();
			double p0_x = sX;
			double p0_y = sY;
			double p1_x = eX;
			double p1_y = sY;
			double p2_x = sX;
			double p2_y = eY;

			// transformation of coordinate system.
			v->viewportToWorld(p0_x, p0_y);
			v->viewportToWorld(p1_x, p1_y);
			v->viewportToWorld(p2_x, p2_y);

			// parameter for SelectCtrlPointsRegion
			QVector2D startP = QVector2D(p0_x, p0_y);
			QVector2D vec1 = QVector2D(p1_x - p0_x, p1_y - p0_y);
			QVector2D vec2 = QVector2D(p2_x - p0_x, p2_y - p0_y);

			box->disable();
			v->restoreUpdateRate();

			// set information of selected control points.
			if (event->modifiers() == Qt::NoModifier) {
				this->ctrlPointSelectRegion(startP, vec1, vec2);
			} else if (event->modifiers() == Qt::ShiftModifier) {
				this->ctrlPointXORSelectRegion(startP, vec1, vec2);
			}

			// no point is selected. try to select a ctrlzone.
			if (! m_selectedCtrlPointInfoList.empty()) {
				m_selectedZone.point = nullptr;
			} else {
				double target_x = box->startPoint().x();
				double target_y = box->startPoint().y();
				v->viewportToWorld(target_x, target_y);
				QVector2D targetP = QVector2D(target_x, target_y);
				selectCtrlZone(targetP, graphicsView()->stdRadius(5));
			}

			setActionStatus();
			updateShapeData(true);
			renderGraphicsView();
		} else if (m_mouseEventMode == meMove) {
			// end the mouse operation
			double worldX = (double) event->x();
			double worldY = (double) event->y();
			v->viewportToWorld(worldX, worldY);
			QVector2D pvec(worldX, worldY);

			double startX = m_dragStartPoint.x();
			double startY = m_dragStartPoint.y();
			v->viewportToWorld(startX, startY);
			QVector2D startP(startX, startY);
			QVector2D delta = pvec - startP;
			double tmpdbl = QVector2D::dotProduct(delta, m_GCPOffsetInfo.direction) / m_GCPOffsetInfo.length;
			if (tmpdbl < m_GCPOffsetInfo.lowerLimit) {tmpdbl = m_GCPOffsetInfo.lowerLimit;}
			if (tmpdbl > m_GCPOffsetInfo.upperLimit) {tmpdbl = m_GCPOffsetInfo.upperLimit;}
			iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointMoveCommand(false, tmpdbl, this, true));

			m_mouseEventMode = meMovePrepare;
			updateMouseCursor(v);
			return;
		}
	} else if (event->button() == Qt::RightButton) {
		if (isNear(m_dragStartPoint, event->pos())) {
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionRiverSurvey::updateMouseCursor(PreProcessorGraphicsViewInterface* v)
{
	switch (m_mouseEventMode) {
	case meMovePrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case meMove:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	default:
		v->setCursor(Qt::ArrowCursor);
		break;
	}

}

void GridCreatingConditionRiverSurvey::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomElement elem = node.toElement();
	m_lastStartPointName = elem.attribute("lastStartPoint");
	m_lastEndPointName = elem.attribute("lastEndPoint");
}

void GridCreatingConditionRiverSurvey::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	if (m_lastStartPoint != nullptr) {
		writer.writeAttribute("lastStartPoint", m_lastStartPoint->name());
	}
	if (m_lastEndPoint != nullptr) {
		writer.writeAttribute("lastEndPoint", m_lastEndPoint->name());
	}
}

void GridCreatingConditionRiverSurvey::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(3);
}

void GridCreatingConditionRiverSurvey::assignActorZValues(const ZDepthRange& range)
{
	double paintDepth = range.min();
	double lineDepth = range.min() * 0.9 + range.max() * 0.1;
	double bigpointDepth = (range.min() + range.max()) * 0.5;
	double pointDepth = range.min() * 0.2 + range.max() * 0.8;
	double selectedBigPointDepth = range.min() * 0.1 + range.max() * 0.9;
	double selectedPointDepth = range.min() * 0.05 + range.max() * 0.95;
	double analogyBigPointDepth = range.min() * 0.03 + range.max() * 0.97;
	double analogyCtrlPointDepth = range.max();

	m_createRegionActor->SetPosition(0, 0, paintDepth);
	m_selectedCtrlZoneActor->SetPosition(0, 0, paintDepth);

	m_crossSectionsActor->SetPosition(0, 0, lineDepth);
	m_riverCenterLineActor->SetPosition(0, 0, lineDepth);
	m_leftBankLineActor->SetPosition(0, 0, lineDepth);
	m_rightBankLineActor->SetPosition(0, 0, lineDepth);

	m_blackFixedPointsActor->SetPosition(0, 0, bigpointDepth);
	m_blackCtrlPointsActor->SetPosition(0, 0, bigpointDepth);

	m_fixedPointsActor->SetPosition(0, 0, pointDepth);
	m_ctrlPointsActor->SetPosition(0, 0, pointDepth);

	m_blackSelectedCtrlPointsActor->SetPosition(0, 0, selectedBigPointDepth);
	m_selectedCtrlPointsActor->SetPosition(0, 0, selectedPointDepth);

	m_blackAnalogyCtrlPointsActor->SetPosition(0, 0, analogyBigPointDepth);
	m_analogyCtrlPointsActor->SetPosition(0, 0, analogyCtrlPointDepth);
}

bool GridCreatingConditionRiverSurvey::addToolBarButtons(QToolBar* /*tb*/)
{
	return true;
}

void GridCreatingConditionRiverSurvey::loadExternalData(const QString& /*filename*/)
{}

void GridCreatingConditionRiverSurvey::saveExternalData(const QString& /*filename*/) {}

void GridCreatingConditionRiverSurvey::setupActions()
{
	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	m_createAction = p->createAction();

	m_addAction = new QAction(tr("&Add Division Points..."), this);
	m_addRegionallyAction = new QAction(tr("Add Division Points &Regionally..."), this);
	m_moveAction = new QAction(tr("&Move Division Points..."), this);
	m_repositionAction = new QAction(tr("Re&position Division Points..."), this);
	m_deleteAction = new QAction(tr("R&emove Division Points..."), this);

	connect(m_addAction, SIGNAL(triggered()), this, SLOT(addPoints()));
	connect(m_addRegionallyAction, SIGNAL(triggered()), this, SLOT(addRegionPoints()));
	connect(m_moveAction, SIGNAL(triggered()), this, SLOT(movePoints()));
	connect(m_repositionAction, SIGNAL(triggered()), this, SLOT(repositionPoints()));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deletePoints()));
}

void GridCreatingConditionRiverSurvey::setActionStatus()
{
	// menu status when zone and points are not selected.

	if ((m_selectedZone.point == nullptr) && (m_selectedCtrlPointInfoList.empty())) {
		m_createAction->setEnabled(true);
		m_addAction->setEnabled(false);
		m_addRegionallyAction->setEnabled(true);
		m_moveAction->setEnabled(false);
		m_repositionAction->setEnabled(false);
		m_deleteAction->setEnabled(false);
	} else {
		m_createAction->setEnabled(true);
		m_addRegionallyAction->setEnabled(false);

		// menu status when zone is selected.
		if (m_selectedZone.point != nullptr) {
			m_addAction->setEnabled(true);
			m_moveAction->setEnabled(false);
			m_repositionAction->setEnabled(false);
			m_deleteAction->setEnabled(false);
		}

		// menu status when points are selected.
		if (! m_selectedCtrlPointInfoList.empty()) {
			m_addAction->setEnabled(false);
			if (this->isCtrlPointsContinuousSelection()) {
				m_moveAction->setEnabled(true);
				m_repositionAction->setEnabled(true);
			} else {
				m_moveAction->setEnabled(false);
				m_repositionAction->setEnabled(false);
			}
			m_deleteAction->setEnabled(true);
		}
	}
}

void GridCreatingConditionRiverSurvey::addRegionPoints()
{
	GridCreatingConditionRiverSurveyPointRegionAddDialog* dialog = new GridCreatingConditionRiverSurveyPointRegionAddDialog(this, preProcessorWindow());
	dialog->setData(m_riverSurvey);
	dialog->setStartPoint(m_lastRegionAddStartPoint);
	dialog->setEndPoint(m_lastRegionAddEndPoint);
	dialog->update();

	m_mouseEventMode = mePointRegionAddDialog;
	iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(hideCreateRegion()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));

	m_createRegionActor->VisibilityOn();
	dialog->show();

}

void GridCreatingConditionRiverSurvey::addPoints()
{
	GridCreatingConditionRiverSurveyPointAddDialog* dlg = new GridCreatingConditionRiverSurveyPointAddDialog(this, preProcessorWindow());
	m_mouseEventMode = meAddPointDialog;

	iricMainWindow()->enterModelessDialogMode();
	dlg->show();
	connect(dlg, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dlg, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
}

void GridCreatingConditionRiverSurvey::movePoints()
{
	GridCreatingConditionRiverSurveyPointMoveDialog* dialog =
		new GridCreatingConditionRiverSurveyPointMoveDialog(
		this, m_GCPOffsetInfo.lowerLimit, m_GCPOffsetInfo.upperLimit, preProcessorWindow());
	m_mouseEventMode = meMovePointDialog;

	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GridCreatingConditionRiverSurvey::repositionPoints()
{
	GridCreatingConditionRiverSurveyPointRepositionDialog* dialog =
		new GridCreatingConditionRiverSurveyPointRepositionDialog(this, preProcessorWindow());
	m_mouseEventMode = meRepositionPointDialog;

	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GridCreatingConditionRiverSurvey::deletePoints()
{
	setAnalogyCtrlPoints();
	updateShapeData(true);

	QMessageBox::StandardButton button = QMessageBox::warning(
		preProcessorWindow(), tr("Confirmation"),
		tr("Are you sure you want to delete these control points?"),
		(QMessageBox::Yes | QMessageBox::No),
		QMessageBox::No);

	if (button == QMessageBox::Yes) {
		iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointDeleteCommand(this));
	}

	m_analogyCtrlPointInfoList.clear();
	invalidateSelectedCtrlPoints();
	setActionStatus();
	updateShapeData(true);
}

bool GridCreatingConditionRiverSurvey::ctrlPointSelectRegion(
	const QVector2D& p0, const QVector2D& v0, const QVector2D& v1)
{
	m_selectedCtrlPointInfoList.clear();

	RawDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	p->SelectCtrlPointsRegion(p0, v0, v1, m_selectedCtrlPointInfoList);

	invalidateSelectedCtrlPoints();
	informCtrlPointUpdateToCrosssectionWindows();
	return (! m_selectedCtrlPointInfoList.empty());
}

bool GridCreatingConditionRiverSurvey::ctrlPointXORSelectRegion(
	const QVector2D& p0, const QVector2D& v0, const QVector2D& v1)
{
	std::list<CtrlPointSelectionInfo> tmplist;
	RawDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	p->SelectCtrlPointsRegion(p0, v0, v1, tmplist);

	for (auto it = m_selectedCtrlPointInfoList.begin(); it != m_selectedCtrlPointInfoList.end(); ++it) {
		bool found = false;
		for (auto it2 = tmplist.begin(); ! found && it2 != tmplist.end(); ++it2) {
			if (*it == *it2) {
				found = true;
				tmplist.erase(it2);
			}
		}
		if (! found) {
			tmplist.push_back(*it);
		}
	}
	m_selectedCtrlPointInfoList = tmplist;

	invalidateSelectedCtrlPoints();
	informCtrlPointUpdateToCrosssectionWindows();
	return (! m_selectedCtrlPointInfoList.empty());
}

void GridCreatingConditionRiverSurvey::informCtrlPointUpdateToCrosssectionWindows()
{
	m_riverSurvey->informCtrlPointUpdateToCrosssectionWindows();
}

void GridCreatingConditionRiverSurvey::setAnalogyCtrlPoints()
{
	m_analogyCtrlPointInfoList.clear();

	typedef std::set<int> iset;
	iset EmptyIndices;
	iset LeftRemoveIndices;
	iset RightRemoveIndices;

	std::set<RawDataRiverPathPoint*> points;
	std::map<RawDataRiverPathPoint*, iset> PointIndices;

	bool RemoveCenterToLeft = false;
	bool RemoveCenterToRight = false;

	// Search Control Points relative to selected Control Points.
	for (auto it = m_selectedCtrlPointInfoList.begin(); it != m_selectedCtrlPointInfoList.end(); ++it) {
		if (it->Position == RawDataRiverPathPoint::pposCenterToLeft) {
			RemoveCenterToLeft = true;
			LeftRemoveIndices.insert(it->Index);
		} else if (it->Position == RawDataRiverPathPoint::pposCenterToRight) {
			RemoveCenterToRight = true;
			RightRemoveIndices.insert(it->Index);
		} else {
			points.insert(it->Point);
			auto psetit = PointIndices.find(it->Point);
			if (psetit == PointIndices.end()) {
				std::pair<RawDataRiverPathPoint*, iset> pi_pair =
					std::pair<RawDataRiverPathPoint*, iset>(it->Point, EmptyIndices);
				auto insertresult = PointIndices.insert(pi_pair);
				psetit = insertresult.first;
			}
			psetit->second.insert(it->Index);
		}
	}

	// Save Control Point Information of above Control Points. ---> m_analogyCtrlPointInfoList
	if (RemoveCenterToLeft) {
		RawDataRiverPathPoint* tmpp = m_riverSurvey->headPoint();
		if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
		while (tmpp != nullptr) {
			for (auto itt = LeftRemoveIndices.begin(); itt != LeftRemoveIndices.end(); ++itt) {
				CtrlPointSelectionInfo sinfo;
				sinfo.Point = tmpp;
				sinfo.Position = RawDataRiverPathPoint::pposCenterToLeft;
				sinfo.Index = *itt;
				m_analogyCtrlPointInfoList.push_back(sinfo);
			}
			tmpp = tmpp->nextPoint();
		}
	}
	if (RemoveCenterToRight) {
		RawDataRiverPathPoint* tmpp = m_riverSurvey->headPoint();
		if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
		while (tmpp != nullptr) {
			for (auto itt = RightRemoveIndices.begin(); itt != RightRemoveIndices.end(); ++itt) {
				CtrlPointSelectionInfo sinfo;
				sinfo.Point = tmpp;
				sinfo.Position = RawDataRiverPathPoint::pposCenterToRight;
				sinfo.Index = *itt;
				m_analogyCtrlPointInfoList.push_back(sinfo);
			}
			tmpp = tmpp->nextPoint();
		}
	}
	for (auto psetit = PointIndices.begin(); psetit != PointIndices.end(); ++psetit) {
		CtrlPointSelectionInfo sinfo;
		sinfo.Point = psetit->first;

		sinfo.Position = RawDataRiverPathPoint::pposLeftBank;
		for (auto itt = psetit->second.begin(); itt != psetit->second.end(); ++itt) {
			sinfo.Index = *itt;
			m_analogyCtrlPointInfoList.push_back(sinfo);
		}
		sinfo.Position = RawDataRiverPathPoint::pposCenterLine;
		for (auto itt = psetit->second.begin(); itt != psetit->second.end(); ++itt) {
			sinfo.Index = *itt;
			m_analogyCtrlPointInfoList.push_back(sinfo);
		}
		sinfo.Position = RawDataRiverPathPoint::pposRightBank;
		for (auto itt = psetit->second.begin(); itt != psetit->second.end(); ++itt) {
			sinfo.Index = *itt;
			m_analogyCtrlPointInfoList.push_back(sinfo);
		}
	}

}

void GridCreatingConditionRiverSurvey::updateShapeData()
{
	updateShapeData(false);
}

void GridCreatingConditionRiverSurvey::updateShapeData(bool omitBackgroundUpdate)
{
	m_crossSectionPoints->Reset();      // points which consist of cross sections.
	m_ctrlPointPoints->Reset();         // control points.
	m_selectedCtrlPointPoints->Reset(); // selected control points.
	m_analogyCtrlPointPoints->Reset();  // control points relative to selected control points.

	m_leftBankLine->Initialize();
	m_rightBankLine->Initialize();
	m_riverCenterLine->Initialize();

	vtkSmartPointer<vtkPoints> centerLinePoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> leftBankPoints = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkPoints> rightBankPoints = vtkSmartPointer<vtkPoints>::New();

	// calculate the number of grid size of m_riverCenterLine etc.
	int pointCount = 0;
	RawDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	while (p != nullptr) {
		++pointCount;
		p = p->nextPoint();
	}
	int gridSize = pointCount + (pointCount - 1) * (LINEDIVS - 1);
	m_riverCenterLine->SetDimensions(gridSize, 1, 1);
	m_leftBankLine->SetDimensions(gridSize, 1, 1);
	m_rightBankLine->SetDimensions(gridSize, 1, 1);

	double point[3];
	point[2] = 0;

	vtkVertex* vtx;
	vtkLine* line;

	m_fixedPoints->Reset();
	m_crossSections->Reset();

	int index = 0;
	p = m_riverSurvey->headPoint()->nextPoint();
	while (p != nullptr) {

		//********************
		// left bank
		//********************

		// Add point information
		QVector2D leftBank = p->crosssectionPosition(p->crosssection().leftBank(true).position());
		point[0] = leftBank.x();
		point[1] = leftBank.y();
		m_crossSectionPoints->InsertNextPoint(point);


		// Add fixed data that represents crosssection
		vtx = vtkVertex::New();
		vtx->GetPointIds()->SetId(0, index);
		m_fixedPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
		vtx->Delete();

		++index;

		//********************
		// left fixed point
		//********************

		// Add point information
		QVector2D leftFixed;
		if (p->crosssection().fixedPointLSet()) {
			leftFixed = p->crosssectionPosition(p->crosssection().fixedPointL().position());
		} else {
			leftFixed = p->crosssectionPosition(p->crosssection().leftBank(true).position());       // use left bank.
		}
		point[0] = leftFixed.x();
		point[1] = leftFixed.y();
		m_crossSectionPoints->InsertNextPoint(point);

		// Add fixed data that represents crosssection
		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		m_crossSections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		line->Delete();

		++index;

		//********************
		// river center
		//********************

		// Add point information
		point[0] = p->position().x();
		point[1] = p->position().y();
		m_crossSectionPoints->InsertNextPoint(point);

		// Add fixed data that represents crosssection
		vtx = vtkVertex::New();
		vtx->GetPointIds()->SetId(0, index);
		m_fixedPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
		vtx->Delete();

		// Add line that represents crosssection
		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		m_crossSections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		line->Delete();

		++index;

		//********************
		// right fixed point
		//********************

		// Add point information
		QVector2D rightFixed;
		if (p->crosssection().fixedPointRSet()) {
			rightFixed = p->crosssectionPosition(p->crosssection().fixedPointR().position());
		} else {
			rightFixed = p->crosssectionPosition(p->crosssection().rightBank(true).position());     // use right bank.
		}
		point[0] = rightFixed.x();
		point[1] = rightFixed.y();
		m_crossSectionPoints->InsertNextPoint(point);

		// Add fixed data that represents crosssection
		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		m_crossSections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		line->Delete();

		++index;

		//********************
		// right bank
		//********************

		// Add point information
		QVector2D rightBank = p->crosssectionPosition(p->crosssection().rightBank(true).position());
		point[0] = rightBank.x();
		point[1] = rightBank.y();
		m_crossSectionPoints->InsertNextPoint(point);

		// Add fixed data that represents crosssection
		vtx = vtkVertex::New();
		vtx->GetPointIds()->SetId(0, index);
		m_fixedPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
		vtx->Delete();

		// Add line that represents crosssection
		line = vtkLine::New();
		line->GetPointIds()->SetId(0, index - 1);
		line->GetPointIds()->SetId(1, index);
		m_crossSections->InsertNextCell(line->GetCellType(), line->GetPointIds());
		line->Delete();

		++index;

		p = p->nextPoint();
	}

	// Draw control points
	m_ctrlPoints->Reset();
	int ctrlPointIndex = 0;

	p = m_riverSurvey->headPoint()->nextPoint();
	while (p != 0) {

		// control points on crosssection (left bank side)
		for (double ctrlP : p->CenterToLeftCtrlPoints) {
			QVector2D vec = p->CtrlPointPosition2D(RawDataRiverPathPoint::pposCenterToLeft, ctrlP);
			point[0] = vec.x();
			point[1] = vec.y();
			m_ctrlPointPoints->InsertNextPoint(point);

			vtx = vtkVertex::New();
			vtx->GetPointIds()->SetId(0, ctrlPointIndex);
			m_ctrlPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
			vtx->Delete();

			++ctrlPointIndex;
		}

		// control points on crosssection (right bank side)
		for (double ctrlP : p->CenterToRightCtrlPoints) {
			QVector2D vec = p->CtrlPointPosition2D(RawDataRiverPathPoint::pposCenterToRight, ctrlP);
			point[0] = vec.x();
			point[1] = vec.y();
			m_ctrlPointPoints->InsertNextPoint(point);

			vtx = vtkVertex::New();
			vtx->GetPointIds()->SetId(0, ctrlPointIndex);
			m_ctrlPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
			vtx->Delete();

			++ctrlPointIndex;
		}

		if (p->nextPoint() != nullptr) {

			// control points on center line
			for (double ctrlP : p->CenterLineCtrlPoints) {
				QVector2D vec = p->CtrlPointPosition2D(RawDataRiverPathPoint::pposCenterLine, ctrlP);
				point[0] = vec.x();
				point[1] = vec.y();
				m_ctrlPointPoints->InsertNextPoint(point);

				vtx = vtkVertex::New();
				vtx->GetPointIds()->SetId(0, ctrlPointIndex);
				m_ctrlPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
				vtx->Delete();

				++ctrlPointIndex;
			}

			// control points on left bank.
			for (double ctrlP : p->LeftBankCtrlPoints) {
				QVector2D vec = p->CtrlPointPosition2D(RawDataRiverPathPoint::pposLeftBank, ctrlP);
				point[0] = vec.x();
				point[1] = vec.y();
				m_ctrlPointPoints->InsertNextPoint(point);

				vtx = vtkVertex::New();
				vtx->GetPointIds()->SetId(0, ctrlPointIndex);
				m_ctrlPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
				vtx->Delete();

				++ctrlPointIndex;
			}

			// control points on right bank.
			for (double ctrlP : p->RightBankCtrlPoints) {
				QVector2D vec = p->CtrlPointPosition2D(RawDataRiverPathPoint::pposRightBank, ctrlP);
				point[0] = vec.x();
				point[1] = vec.y();
				m_ctrlPointPoints->InsertNextPoint(point);

				vtx = vtkVertex::New();
				vtx->GetPointIds()->SetId(0, ctrlPointIndex);
				m_ctrlPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
				vtx->Delete();

				++ctrlPointIndex;
			}
		}

		QVector2D tmpp;

		// river center line
		point[0] = p->position().x();
		point[1] = p->position().y();
		centerLinePoints->InsertNextPoint(point);
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				tmpp = p->riverCenter()->interpolate(i / static_cast<double>(LINEDIVS));
				point[0] = tmpp.x();
				point[1] = tmpp.y();
				centerLinePoints->InsertNextPoint(point);
			}
		}
		// left bank line
		tmpp = p->crosssectionPosition(p->crosssection().leftBank(true).position());
		point[0] = tmpp.x();
		point[1] = tmpp.y();
		leftBankPoints->InsertNextPoint(point);
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				tmpp = p->leftBank()->interpolate(i / static_cast<double>(LINEDIVS));
				point[0] = tmpp.x();
				point[1] = tmpp.y();
				leftBankPoints->InsertNextPoint(point);
			}
		}
		// right bank line
		tmpp = p->crosssectionPosition(p->crosssection().rightBank(true).position());
		point[0] = tmpp.x();
		point[1] = tmpp.y();
		rightBankPoints->InsertNextPoint(point);
		if (p->nextPoint() != nullptr) {
			for (int i = 1; i < LINEDIVS; ++i) {
				tmpp = p->rightBank()->interpolate(i / static_cast<double>(LINEDIVS));
				point[0] = tmpp.x();
				point[1] = tmpp.y();
				rightBankPoints->InsertNextPoint(point);
			}
		}
		p = p->nextPoint();
	}

	// selected zone
	m_selectedCtrlZone->Initialize();
	if (m_selectedZone.point != nullptr) {
		// zone is selected!
		m_selectedCtrlZone->SetDimensions(ZONEDIV, 1, 1);

		RawDataRiverPathPoint* p = m_selectedZone.point;
		QList<QVector2D> pointlist = p->CtrlZonePoints(m_selectedZone.position, m_selectedZone.index, ZONEDIV);
		m_selectedCtrlZone->SetDimensions(pointlist.count(), 1, 1);
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		for (const QVector2D& v : pointlist) {
			points->InsertNextPoint(v.x(), v.y(), 0);
		}
		m_selectedCtrlZone->SetPoints(points);
		m_selectedCtrlZone->Modified();
	}

	// Show selected ctrl points
	m_selectedCtrlPointPoints->Reset();
	m_selectedCtrlPoints->Reset();

	int selectedCtrlPointIndex = 0;
	for (const CtrlPointSelectionInfo& sInfo : m_selectedCtrlPointInfoList) {
		point[0] = sInfo.Point->CtrlPointPosition2D(sInfo.Position, sInfo.Index).x();
		point[1] = sInfo.Point->CtrlPointPosition2D(sInfo.Position, sInfo.Index).y();
		m_selectedCtrlPointPoints->InsertNextPoint(point);

		vtx = vtkVertex::New();
		vtx->GetPointIds()->SetId(0, selectedCtrlPointIndex);
		m_selectedCtrlPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
		vtx->Delete();

		++selectedCtrlPointIndex;
	}

	// Show ctrl points related to selected ctrl points
	m_analogyCtrlPointPoints->Reset();
	m_analogyCtrlPoints->Reset();

	int analogyCtrlPointIndex = 0;
	for (CtrlPointSelectionInfo& sInfo : m_analogyCtrlPointInfoList) {
		point[0] = sInfo.Point->CtrlPointPosition2D(sInfo.Position, sInfo.Index).x();
		point[1] = sInfo.Point->CtrlPointPosition2D(sInfo.Position, sInfo.Index).y();
		m_analogyCtrlPointPoints->InsertNextPoint(point);

		vtx = vtkVertex::New();
		vtx->GetPointIds()->SetId(0, analogyCtrlPointIndex);
		m_analogyCtrlPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
		vtx->Delete();

		++analogyCtrlPointIndex;
	}

	m_crossSectionPoints->Modified();
	m_crossSections->SetPoints(m_crossSectionPoints);
	m_crossSections->Modified();
	m_fixedPoints->SetPoints(m_crossSectionPoints);
	m_fixedPoints->Modified();

	m_ctrlPointPoints->Modified();
	m_ctrlPoints->SetPoints(m_ctrlPointPoints);
	m_ctrlPoints->Modified();

	m_riverCenterLine->SetPoints(centerLinePoints);
	m_riverCenterLine->Modified();
	m_leftBankLine->SetPoints(leftBankPoints);
	m_leftBankLine->Modified();
	m_rightBankLine->SetPoints(rightBankPoints);
	m_rightBankLine->Modified();

	m_selectedCtrlPointPoints->Modified();
	m_selectedCtrlPoints->SetPoints(m_selectedCtrlPointPoints);
	m_selectedCtrlPoints->Modified();

	m_analogyCtrlPointPoints->Modified();
	m_analogyCtrlPoints->SetPoints(m_analogyCtrlPointPoints);
	m_analogyCtrlPoints->Modified();

	if (! omitBackgroundUpdate) {
		m_riverSurvey->refreshBackgroundGrid();
	}
}

void GridCreatingConditionRiverSurvey::updateGridInterpolators()
{
	// clear first.
	for (auto s : m_gridSolvers) {
		delete s;
	}
	m_gridSolvers.clear();
	RawDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	// center to left
	for (int i = 0; i < p->CenterToLeftCtrlPoints.size(); ++i) {
		RiverGridCtrlSolver* solver = new RiverGridCtrlSolver();
		solver->SetBankSide(RiverGridCtrlSolver::bs_LeftBank);
		solver->SetIndex(i);
		solver->setHeadPoint(m_riverSurvey->headPoint());
		m_gridSolvers.push_back(solver);
	}
	// center to right
	for (int i = 0; i < p->CenterToRightCtrlPoints.size(); ++i) {
		RiverGridCtrlSolver* solver = new RiverGridCtrlSolver();
		solver->SetBankSide(RiverGridCtrlSolver::bs_RightBank);
		solver->SetIndex(i);
		solver->setHeadPoint(m_riverSurvey->headPoint());
		m_gridSolvers.push_back(solver);
	}
	// prepare interpolators.
	unsigned int lindices = static_cast<unsigned int>(p->CenterToLeftCtrlPoints.size());
	unsigned int rindices = static_cast<unsigned int>(p->CenterToRightCtrlPoints.size());
	p = m_riverSurvey->headPoint();
	while (p != nullptr) {
		for (auto i : p->LGridLines()) {
			delete i;
		}
		for (auto i : p->RGridLines()) {
			delete i;
		}
		p->LGridLines().clear();
		p->LGridLines().insert(0, lindices, 0);
		p->RGridLines().clear();
		p->RGridLines().insert(0, rindices, 0);
		p = p->nextPoint();
	}
	for (RiverGridCtrlSolver* solver : m_gridSolvers) {
		solver->update();
	}
}

void GridCreatingConditionRiverSurvey::hideCreateRegion()
{
	m_createRegionActor->VisibilityOff();
}

void GridCreatingConditionRiverSurvey::createGrid(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end)
{
	m_lastStartPoint = start;
	m_lastEndPoint = end;

	Structured2DGrid* grid = new Structured2DGrid(nullptr);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridRelatedConditions(grid);

	RawDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	unsigned int dimJ = static_cast<unsigned int>(p->CenterToLeftCtrlPoints.size() + p->CenterToRightCtrlPoints.size() + 3);
	unsigned int dimI = m_lastStartPoint->gridCounts(m_lastEndPoint);

	if (dimI * dimJ > MAXGRIDSIZE) {
		QMessageBox::warning(dataModel()->mainWindow(), tr("Warning"), tr("The maximum number of grid nodes is %1.").arg(MAXGRIDSIZE));
		return;
	}
	grid->setDimensions(dimI, dimJ);
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	points->InsertPoint(dimI * dimJ - 1, 0, 0, 0);
	grid->vtkGrid()->SetPoints(points);

	for (GridAttributeContainer* c : grid->gridRelatedConditions()) {
		c->allocate();
	}
	// update grid interpolator.
	updateGridInterpolators();

	// create grid.
	unsigned int current = 0;
	p = m_lastStartPoint;
	while (p != m_lastEndPoint) {
		p->createGrid(grid, current, false);
		current += static_cast<unsigned int>(p->CenterLineCtrlPoints.size() + 1);
		p = p->nextPoint();
	}
	p->createGrid(grid, current, false, true);
	grid->setModified();
	/*
		GridRelatedConditionContainerT<double>* elev = dynamic_cast<GridRelatedConditionContainerT<double>*>(grid->gridRelatedCondition("Elevation"));
		if (elev != 0 && (RawDataRiverPathPoint::ELEV_MAPPING == 1)){
			elev->setMapped(true);
		}
	*/
	emit gridCreated(grid);
	iRICUndoStack::instance().clear();
}

void GridCreatingConditionRiverSurvey::selectCreateRegion(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end)
{
	m_createRegion->RemoveAllInputs();
	RawDataRiverPathPoint* p = start;
	while (p != end) {
		vtkSmartPointer<vtkGeometryFilter> filter = vtkSmartPointer<vtkGeometryFilter>::New();
		filter->SetInputData(p->areaGrid());
		m_createRegion->AddInputConnection(filter->GetOutputPort());
		p = p->nextPoint();
	}
	renderGraphicsView();
}

void GridCreatingConditionRiverSurvey::allActorsOff()
{
	m_riverCenterLineActor->VisibilityOff();
	m_leftBankLineActor->VisibilityOff();
	m_rightBankLineActor->VisibilityOff();

	m_crossSectionsActor->VisibilityOff();
	m_fixedPointsActor->VisibilityOff();
	m_blackFixedPointsActor->VisibilityOff();
	m_blackCtrlPointsActor->VisibilityOff();
	m_ctrlPointsActor->VisibilityOff();

	m_selectedCtrlPointsActor->VisibilityOff();
	m_blackSelectedCtrlPointsActor->VisibilityOff();

	m_analogyCtrlPointsActor->VisibilityOff();
	m_blackAnalogyCtrlPointsActor->VisibilityOff();

	m_createRegionActor->VisibilityOff();
}

void GridCreatingConditionRiverSurvey::selectCtrlZone(const QVector2D& point, double width)
{
	m_selectedZone.point = nullptr;
	RawDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	double rwidth = width;
	bool found = false;
	bool region;
	while (! found && p != nullptr) {
		if (p->gridSkip()) {
			p = p->nextPoint();
			continue;
		}
		QVector2D mins, maxs;
		p->centerToBanksRegion(mins, maxs);
		double xwidth = maxs.x() - mins.x();
		double ywidth = maxs.y() - mins.y();
		region = true;
		if (xwidth != 0 && (point.x() < mins.x() - xwidth || point.x() > maxs.x() + xwidth)) {
			region = false;
		}
		if (ywidth != 0 && (point.y() < mins.y() - ywidth || point.y() > maxs.y() + ywidth)) {
			region = false;
		}
		if (region) {
			found = found || selectCtrlZone(p, RawDataRiverPathPoint::zposCenterToLeft, point, rwidth);
			found = found || selectCtrlZone(p, RawDataRiverPathPoint::zposCenterToRight, point, rwidth);
		}
		if (p->nextPoint() != nullptr) {
			region = true;
			p->thisToNextRegion(mins, maxs);
			xwidth = maxs.x() - mins.x();
			ywidth = maxs.y() - mins.y();
			if (xwidth != 0 && (point.x() < mins.x() - xwidth || point.x() > maxs.x() + xwidth)) {
				region = false;
			}
			if (ywidth != 0 && (point.y() < mins.y() - ywidth || point.y() > maxs.y() + ywidth)) {
				region = false;
			}
			if (region) {
				found = found || selectCtrlZone(p, RawDataRiverPathPoint::zposCenterLine, point, rwidth);
				found = found || selectCtrlZone(p, RawDataRiverPathPoint::zposLeftBank, point, rwidth);
				found = found || selectCtrlZone(p, RawDataRiverPathPoint::zposRightBank, point, rwidth);
			}
		}
		p = p->nextPoint();
	}
}

bool GridCreatingConditionRiverSurvey::selectCtrlZone(RawDataRiverPathPoint* p, RawDataRiverPathPoint::CtrlZonePosition pos, const QVector2D& point, double width)
{
	bool found = false;
	unsigned int maxindex = static_cast<unsigned int>(p->CtrlPoints(pos).size());
	for (unsigned int i = 0; ! found && i <= maxindex; ++i) {
		QList<QVector2D> vectorlist = p->CtrlZonePoints(pos, i, ZONEDIV);
		QVector2D v0, v1;
		auto it = vectorlist.begin();
		v0 = *it++;
		while (! found && it != vectorlist.end()) {
			v1 = *it++;
			QVector2D horizontal = v1 - v0;
			QVector2D vertical = (v1 - v0).normalized();
			iRIC::rotateVector90(vertical);
			vertical *= width;
			QVector2D posv = v0 - vertical * 0.5;
			if (iRIC::isInsideParallelogram(point, posv, horizontal, vertical)) {
				found = true;
				m_selectedZone.point = p;
				m_selectedZone.position = pos;
				m_selectedZone.index = i;
			}
			v0 = v1;
		}
	}
	return found;
}

void GridCreatingConditionRiverSurvey::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

bool GridCreatingConditionRiverSurvey::isCtrlPointsContinuousSelection()
{
	if (m_selectedCtrlPointInfoList.size() == 0) {
		return false;
	} else if (m_selectedCtrlPointInfoList.size() == 1) {
		return true;
	} else {
		bool ok = true;
		auto it = m_selectedCtrlPointInfoList.begin();
		CtrlPointSelectionInfo tmpinfo = *it++;
		while (it != m_selectedCtrlPointInfoList.end()) {
			ok = ok && (tmpinfo.Point == it->Point && tmpinfo.Index + 1 == it->Index);
			tmpinfo = *it++;
		}
		return ok;
	}
}

void GridCreatingConditionRiverSurvey::invalidateSelectedCtrlPoints()
{
	std::list<CtrlPointSelectionInfo> tmplist;
	for (const CtrlPointSelectionInfo& sInfo : m_selectedCtrlPointInfoList) {
		if (sInfo.Index < sInfo.Point->CtrlPoints(sInfo.Position).size()) {
			// valid
			tmplist.push_back(sInfo);
		}
	}
	m_selectedCtrlPointInfoList = tmplist;
	if (isCtrlPointsContinuousSelection()) {
		CtrlPointSelectionInfo& info = m_selectedCtrlPointInfoList.front();
		if (info.Index == 0) {
			m_GCPOffsetInfo.lowerLimit =
				0.
				- info.Point->CtrlPoints(info.Position)[info.Index];
		} else {
			m_GCPOffsetInfo.lowerLimit =
				info.Point->CtrlPoints(info.Position)[info.Index - 1]
				- info.Point->CtrlPoints(info.Position)[info.Index];
		}
		CtrlPointSelectionInfo& info2 = m_selectedCtrlPointInfoList.back();
		if (info2.Index == info.Point->CtrlPoints(info2.Position).size() - 1) {
			m_GCPOffsetInfo.upperLimit =
				1.
				- info2.Point->CtrlPoints(info2.Position)[info2.Index];
		} else {
			m_GCPOffsetInfo.upperLimit =
				info2.Point->CtrlPoints(info2.Position)[info2.Index + 1]
				- info2.Point->CtrlPoints(info2.Position)[info2.Index];
		}
		double d = info.Point->CtrlPoints(info.Position)[info.Index];
		if ((d + 0.01) > 1) {
			m_GCPOffsetInfo.direction = (
				info.Point->CtrlPointPosition2D(info.Position, d) -
				info.Point->CtrlPointPosition2D(info.Position, d - 0.01)
				).normalized();
		} else {
			m_GCPOffsetInfo.direction = (
				info.Point->CtrlPointPosition2D(info.Position, d + 0.01) -
				info.Point->CtrlPointPosition2D(info.Position, d)
				).normalized();
		}
		if (info.Position == RawDataRiverPathPoint::pposCenterToLeft) {
			m_GCPOffsetInfo.length = std::abs(info.Point->crosssection().leftBank(true).position());
		} else if (info.Position == RawDataRiverPathPoint::pposCenterToRight) {
			m_GCPOffsetInfo.length = std::abs(info.Point->crosssection().rightBank(true).position());
		} else {
			m_GCPOffsetInfo.length = (
				info.Point->nextPoint()->position() - info.Point->position()
				).length();
		}
		m_GCPOffsetInfo.points = info.Point->CtrlPoints(info.Position);
	}
}

bool GridCreatingConditionRiverSurvey::checkCtrlPointsRegion(RawDataRiverPathPoint* start, RawDataRiverPathPoint* end)
{
	bool ans = false;

	RawDataRiverPathPoint* tmpp;
	// check whether data already exists in this region.
	tmpp = start;
	while (tmpp != end) {
		if (tmpp->CenterLineCtrlPoints.size() > 0) {
			ans = true;
		}
		tmpp = tmpp->nextPoint();
	}
	return ans;
}

void GridCreatingConditionRiverSurvey::handleDataChange()
{
	updateShapeData();
}

void GridCreatingConditionRiverSurvey::handleDataDestroy()
{
	// River survey data is destroyed.
	// This algorithm is no longer available.
	PreProcessorGridCreatingConditionDataItemInterface* item = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	item->setCondition(nullptr);
	m_riverSurvey = nullptr;
	delete this;
}

void GridCreatingConditionRiverSurvey::clear()
{
	RawDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	QVector<double> emptyVector;
	while (p != nullptr) {
		p->LeftBankCtrlPoints = emptyVector;
		p->RightBankCtrlPoints = emptyVector;
		p->CenterLineCtrlPoints = emptyVector;
		p->CenterToLeftCtrlPoints = emptyVector;
		p->CenterToRightCtrlPoints = emptyVector;
		p = p->nextPoint();
	}
	m_lastStartPoint = nullptr;
	m_lastEndPoint = nullptr;
	m_selectedCtrlPointInfoList.clear();
	updateShapeData();
}

void GridCreatingConditionRiverSurvey::cancelBackgroundGridUpdate()
{
	m_riverSurvey->cancelBackgroundGridUpdate();
}
