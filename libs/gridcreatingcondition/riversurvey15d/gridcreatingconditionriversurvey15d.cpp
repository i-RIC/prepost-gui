#include "gridcreatingconditionriversurvey15d.h"
#include "gridcreatingconditionriversurvey15dpointadddialog.h"
#include "gridcreatingconditionriversurvey15dpointmovedialog.h"
#include "gridcreatingconditionriversurvey15dpointregionadddialog.h"
#include "gridcreatingconditionriversurvey15dpointrepositiondialog.h"
#include "gridcreatingconditionriversurvey15dregiondialog.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatacomplexgroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/pre/complex/gridcomplexconditiongroup.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured15dgrid/structured15dgridwithcrosssectioncrosssection.h>
#include <guicore/pre/grid/structured15dgridwithcrosssection.h>
#include <guicore/pre/gridcond/container/gridattributerealnodecontainer.h>
#include <guicore/project/inputcond/inputconditioncontainerfunctional.h>
#include <guicore/project/inputcond/inputconditioncontainerreal.h>
#include <guicore/project/inputcond/inputconditioncontainerset.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/mathsupport.h>
#include <misc/xmlsupport.h>
#include <misc/zdepthrange.h>
#include <geodata/riversurvey/geodatariverpathpoint.h>
#include <geodata/riversurvey/geodatarivershapeinterpolator.h>
#include <geodata/riversurvey/geodatariversurvey.h>
#include <geodata/riversurvey/geodatariversurveyctrlpointbackup.h>

#include <QInputEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QToolBar>
#include <QUndoCommand>
#include <QXmlStreamWriter>
#include <QMainWindow>
#
#include <vtkExtractGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkLine.h>
#include <vtkPoints.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkStructuredGridGeometryFilter.h>
#include <vtkVertex.h>

#include <cmath>
#include <vector>

class GridCreatingConditionCtrlPointDeleteCommand15D : public QUndoCommand
{
public:
	GridCreatingConditionCtrlPointDeleteCommand15D(GridCreatingConditionRiverSurvey15D* cond)
		: QUndoCommand(GridCreatingConditionRiverSurvey15D::tr("Delete Selected Control Points")) {
		m_condition = cond;
		executeDeleteCtrlPoints();
	}
	void undo() {
		for (GeoDataRiverSurveyCtrlPointBackup* backup : m_before) {
			backup->restore();
		}
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}

	void redo() {
		for (GeoDataRiverSurveyCtrlPointBackup* backup : m_after) {
			backup->restore();
		}
		m_condition->updateShapeData();
		m_condition->renderGraphicsView();
	}

private:
	void executeDeleteCtrlPoints() {
		GeoDataRiverSurveyCtrlPointBackup* backup;

		typedef std::set<int> iset;
		iset EmptyIndices;
		iset LeftRemoveIndices;
		iset RightRemoveIndices;

		std::set<GeoDataRiverPathPoint*> points;
		std::map<GeoDataRiverPathPoint*, iset> PointIndices;

		bool RemoveCenterToLeft = false;
		bool RemoveCenterToRight = false;

		for (auto it = m_condition->m_selectedCtrlPointInfoList.begin(); it != m_condition->m_selectedCtrlPointInfoList.end(); ++it) {
			if (it->Position == GeoDataRiverPathPoint::pposCenterToLeft) {
				RemoveCenterToLeft = true;
				LeftRemoveIndices.insert(it->Index);
			} else if (it->Position == GeoDataRiverPathPoint::pposCenterToRight) {
				RemoveCenterToRight = true;
				RightRemoveIndices.insert(it->Index);
			} else {
				points.insert(it->Point);
				auto psetit = PointIndices.find(it->Point);
				if (psetit == PointIndices.end()) {
					std::pair<GeoDataRiverPathPoint*, iset> pi_pair =
						std::pair<GeoDataRiverPathPoint*, iset>(it->Point, EmptyIndices);
					auto insertresult =
						PointIndices.insert(pi_pair);
					psetit = insertresult.first;
				}
				psetit->second.insert(it->Index);
			}
		}

		GeoDataRiverPathPoint* headPoint = m_condition->m_riverSurvey->headPoint();

		// Save backup
		if (RemoveCenterToLeft) {
			backup = new GeoDataRiverSurveyCtrlPointBackup();
			backup->backup(headPoint, GeoDataRiverPathPoint::zposCenterToLeft);
			m_before.push_back(backup);
		}
		if (RemoveCenterToRight) {
			backup = new GeoDataRiverSurveyCtrlPointBackup();
			backup->backup(headPoint, GeoDataRiverPathPoint::zposCenterToRight);
			m_before.push_back(backup);
		}
		for (GeoDataRiverPathPoint* point : points) {
			backup = new GeoDataRiverSurveyCtrlPointBackup();
			backup->backup(point, GeoDataRiverPathPoint::zposCenterLine);
			m_before.push_back(backup);
		}

		// Delete the points
		if (RemoveCenterToLeft) {
			GeoDataRiverPathPoint* tmpp = m_condition->m_riverSurvey->headPoint();
			if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
			while (tmpp != nullptr) {
				tmpp->removeCtrlPoints(GeoDataRiverPathPoint::zposCenterToLeft, LeftRemoveIndices);
				tmpp = tmpp->nextPoint();
			}
		}
		if (RemoveCenterToRight) {
			GeoDataRiverPathPoint* tmpp = m_condition->m_riverSurvey->headPoint();
			if (tmpp != nullptr) {tmpp = tmpp->nextPoint();}
			while (tmpp != nullptr) {
				tmpp->removeCtrlPoints(GeoDataRiverPathPoint::zposCenterToRight, RightRemoveIndices);
				tmpp = tmpp->nextPoint();
			}
		}
		for (auto psetit = PointIndices.begin(); psetit != PointIndices.end(); ++psetit) {
			(psetit->first)->removeCtrlPoints(GeoDataRiverPathPoint::zposCenterLine, psetit->second);
		}

		// Save backup.
		if (RemoveCenterToLeft) {
			backup = new GeoDataRiverSurveyCtrlPointBackup();
			backup->backup(headPoint, GeoDataRiverPathPoint::zposCenterToLeft);
			m_after.push_back(backup);
		}
		if (RemoveCenterToRight) {
			backup = new GeoDataRiverSurveyCtrlPointBackup();
			backup->backup(headPoint, GeoDataRiverPathPoint::zposCenterToRight);
			m_after.push_back(backup);
		}
		for (auto pit = points.begin(); pit != points.end(); ++pit) {
			backup = new GeoDataRiverSurveyCtrlPointBackup();
			backup->backup(*pit, GeoDataRiverPathPoint::zposCenterLine);
			m_after.push_back(backup);
		}

		m_condition->m_selectedCtrlPointInfoList.clear();
	}

private:
	GridCreatingConditionRiverSurvey15D* m_condition;
	std::list<GeoDataRiverSurveyCtrlPointBackup*> m_before;
	std::list<GeoDataRiverSurveyCtrlPointBackup*> m_after;
};


// constructor
GridCreatingConditionRiverSurvey15D::GridCreatingConditionRiverSurvey15D(ProjectDataItem* parent, GridCreatingConditionCreator* creator)
	: GridCreatingCondition(parent, creator)
{
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

GridCreatingConditionRiverSurvey15D::~GridCreatingConditionRiverSurvey15D()
{
	vtkRenderer* r = renderer();

	r->RemoveActor(m_riverCenterLineActor);
	r->RemoveActor(m_fixedPointsActor);
	r->RemoveActor(m_blackFixedPointsActor);
	r->RemoveActor(m_ctrlPointsActor);
	r->RemoveActor(m_blackCtrlPointsActor);
	r->RemoveActor(m_selectedCtrlZoneActor);
	r->RemoveActor(m_createRegionActor);

	r->RemoveActor(m_selectedCtrlPointsActor);
	r->RemoveActor(m_blackSelectedCtrlPointsActor);

	delete m_rightClickingMenu;
}

bool GridCreatingConditionRiverSurvey15D::create(QWidget* /*parent*/)
{
	GridCreatingConditionRiverSurvey15DRegionDialog* dialog = new GridCreatingConditionRiverSurvey15DRegionDialog(this, preProcessorWindow());
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

bool GridCreatingConditionRiverSurvey15D::ready() const
{
	return true;
}

bool GridCreatingConditionRiverSurvey15D::init()
{
	// set m_riverSurvey.
	PreProcessorGridTypeDataItemInterface* gtItem = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(parent()->parent()->parent());
	PreProcessorGeoDataTopDataItemInterface* rtItem = gtItem->geoDataTop();
	QList<PreProcessorGeoDataGroupDataItemInterface*> gItems = rtItem->groupDataItems();
	bool found = false;

	for (auto git = gItems.begin(); ! found && git != gItems.end(); ++git) {
		PreProcessorGeoDataGroupDataItemInterface* gItem = *git;
		QList<PreProcessorGeoDataDataItemInterface*> rItems = gItem->geoDatas();
		for (auto rit = rItems.begin(); ! found && rit != rItems.end(); ++rit) {
			PreProcessorGeoDataDataItemInterface* rItem = *rit;
			if (dynamic_cast<GeoDataRiverSurvey*>(rItem->geoData()) != nullptr) {
				// this is a river survey data!

				// @todo currently, the river survey data found first, is
				// automatically selected.
				// For cases when there are multiple river survey data, this
				// implementation should be changed.
				m_riverSurvey = dynamic_cast<GeoDataRiverSurvey*>(rItem->geoData());
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
	// delete division points on cross sections
	GeoDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	while (p != nullptr) {
		p->CenterToLeftCtrlPoints.clear();
		p->CenterToRightCtrlPoints.clear();
		p = p->nextPoint();
	}
	updateShapeData();
	return true;
}

void GridCreatingConditionRiverSurvey15D::setupVtkContainers()
{
	// setup points.
	m_centerFixedPoints = vtkSmartPointer<vtkPoints>::New();            // fixed points on center lines.
	m_centerFixedPoints->SetDataTypeToDouble();
	m_ctrlPointPoints = vtkSmartPointer<vtkPoints>::New();              // control points set for grid generation.
	m_ctrlPointPoints->SetDataTypeToDouble();
	m_selectedCtrlPointPoints = vtkSmartPointer<vtkPoints>::New();      // selected control points
	m_selectedCtrlPointPoints->SetDataTypeToDouble();

	// setup grid. (for graphic data)
	m_riverCenterLine = vtkSmartPointer<vtkStructuredGrid>::New();

	m_fixedPoints = vtkSmartPointer<vtkUnstructuredGrid>::New();            // points which consist of river survey lines.
	m_ctrlPoints = vtkSmartPointer<vtkUnstructuredGrid>::New();             // control points.
	m_selectedCtrlPoints = vtkSmartPointer<vtkUnstructuredGrid>::New();     // selected control points
	m_selectedCtrlZone = vtkSmartPointer<vtkStructuredGrid>::New();         // selected zone

	m_createRegion = vtkSmartPointer<vtkAppendPolyData>::New();
}

void GridCreatingConditionRiverSurvey15D::setupActors()
{
	vtkSmartPointer<vtkDataSetMapper> mapper;
	vtkRenderer* r = renderer();

	// fixed points
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
	m_blackFixedPointsActor->GetProperty()->SetColor(0, 0, 0);              // black rim.
	m_blackFixedPointsActor->GetProperty()->SetPointSize(3);
	r->AddActor(m_blackFixedPointsActor);

	// cntrol points
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
	m_blackCtrlPointsActor->GetProperty()->SetColor(0, 0, 0);               // black rim.
	m_blackCtrlPointsActor->GetProperty()->SetPointSize(3);
	r->AddActor(m_blackCtrlPointsActor);

	// selected division points
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputData(m_selectedCtrlPoints);

	m_selectedCtrlPointsActor = vtkSmartPointer<vtkActor>::New();
	m_selectedCtrlPointsActor->SetMapper(mapper);
	m_selectedCtrlPointsActor->GetProperty()->SetColor(1, 1, 0);            // yellow.
	m_selectedCtrlPointsActor->GetProperty()->SetPointSize(7);              // selected division point size is 7 pixel.
	r->AddActor(m_selectedCtrlPointsActor);

	m_blackSelectedCtrlPointsActor = vtkSmartPointer<vtkActor>::New();
	m_blackSelectedCtrlPointsActor->SetMapper(mapper);
	m_blackSelectedCtrlPointsActor->GetProperty()->SetColor(0, 0, 0);       // black rim.
	m_blackSelectedCtrlPointsActor->GetProperty()->SetPointSize(9);         // selected division point size is 9 pixel.
	r->AddActor(m_blackSelectedCtrlPointsActor);

	// center line
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

	// selected cntrol zone
	f = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	f->SetInputData(m_selectedCtrlZone);

	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(f->GetOutputPort());

	m_selectedCtrlZoneActor = vtkSmartPointer<vtkActor>::New();
	m_selectedCtrlZoneActor->SetMapper(mapper);
	m_selectedCtrlZoneActor->GetProperty()->SetColor(0, 0, 1.);
	m_selectedCtrlZoneActor->GetProperty()->SetLineWidth(5);
	m_selectedCtrlZoneActor->GetProperty()->SetOpacity(.5);
	m_selectedCtrlZoneActor->GetProperty()->SetLighting(false);
	r->AddActor(m_selectedCtrlZoneActor);

	// create region
	mapper = vtkSmartPointer<vtkDataSetMapper>::New();
	mapper->SetInputConnection(m_createRegion->GetOutputPort());

	m_createRegionActor = vtkSmartPointer<vtkActor>::New();
	m_createRegionActor->SetMapper(mapper);
	m_createRegionActor->GetProperty()->SetColor(0, 0, 1.);
	m_createRegionActor->GetProperty()->SetLineWidth(5);
	m_createRegionActor->GetProperty()->SetOpacity(.5);
	m_createRegionActor->GetProperty()->SetLighting(false);
	m_createRegionActor->VisibilityOff();
	r->AddActor(m_createRegionActor);

	actorCollection()->AddItem(m_fixedPointsActor);
	actorCollection()->AddItem(m_ctrlPointsActor);
	actorCollection()->AddItem(m_selectedCtrlPointsActor);
	actorCollection()->AddItem(m_blackFixedPointsActor);
	actorCollection()->AddItem(m_blackCtrlPointsActor);
	actorCollection()->AddItem(m_blackSelectedCtrlPointsActor);
	actorCollection()->AddItem(m_riverCenterLineActor);
	actorCollection()->AddItem(m_selectedCtrlZoneActor);
}

void GridCreatingConditionRiverSurvey15D::setupMenu()
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

	// ContextMenu
	if (m_rightClickingMenu == nullptr) {
		m_rightClickingMenu = new QMenu();
		m_rightClickingMenu->addAction(m_createAction);
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_addAction);
		m_rightClickingMenu->addAction(m_addRegionallyAction);
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_deleteAction);
		m_rightClickingMenu->addSeparator();
		m_rightClickingMenu->addAction(m_moveAction);
		m_rightClickingMenu->addAction(m_repositionAction);
		m_rightClickingMenu->addSeparator();
	}
}

void GridCreatingConditionRiverSurvey15D::informSelection(PreProcessorGraphicsViewInterface* /*v*/)
{
	updateShapeData();
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	col->RemoveAllItems();
	col->AddItem(m_fixedPointsActor);
	col->AddItem(m_ctrlPointsActor);
	col->AddItem(m_selectedCtrlPointsActor);
	col->AddItem(m_blackFixedPointsActor);
	col->AddItem(m_blackCtrlPointsActor);
	col->AddItem(m_blackSelectedCtrlPointsActor);
	col->AddItem(m_riverCenterLineActor);
	updateVisibility();
}

void GridCreatingConditionRiverSurvey15D::informDeselection(PreProcessorGraphicsViewInterface* /*v*/)
{
	allActorsOff();
	vtkActorCollection* col = actorCollection();
	col->RemoveAllItems();
	col->AddItem(m_fixedPointsActor);
	col->AddItem(m_ctrlPointsActor);
	col->AddItem(m_selectedCtrlPointsActor);
	col->AddItem(m_blackFixedPointsActor);
	col->AddItem(m_blackCtrlPointsActor);
	col->AddItem(m_blackSelectedCtrlPointsActor);
	col->AddItem(m_riverCenterLineActor);
	updateVisibility();
}

void GridCreatingConditionRiverSurvey15D::viewOperationEnded(PreProcessorGraphicsViewInterface* v)
{
	updateMouseCursor(v);
}

void GridCreatingConditionRiverSurvey15D::keyPressEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionRiverSurvey15D::keyReleaseEvent(QKeyEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionRiverSurvey15D::mouseDoubleClickEvent(QMouseEvent* /*event*/, PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionRiverSurvey15D::mouseMoveEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
{

	if (m_mouseEventMode == meNormal) {
		double worldX = (double) event->x();
		double worldY = (double) event->y();
		v->viewportToWorld(worldX, worldY);

		//if (m_definingBoundingBox){
		// drawing bounding box using mouse dragging.
		MouseBoundingBox* box = dataModel()->mouseBoundingBox();
		box->setEndPoint(event->x(), event->y());
		renderGraphicsView();
		//}
	}
}

void GridCreatingConditionRiverSurvey15D::mousePressEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* /*v*/)
{
	if (event->button() == Qt::LeftButton) {
		if (m_mouseEventMode == meNormal) {
			// Bounding box for selection started.
			MouseBoundingBox* box = dataModel()->mouseBoundingBox();
			box->setStartPoint(event->x(), event->y());
			box->enable();
			renderGraphicsView();
		}
	}
	m_dragStartPoint = event->pos();
}

void GridCreatingConditionRiverSurvey15D::mouseReleaseEvent(QMouseEvent* event, PreProcessorGraphicsViewInterface* v)
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
			QPointF startP = QPointF(p0_x, p0_y);
			QPointF vec1 = QPointF(p1_x - p0_x, p1_y - p0_y);
			QPointF vec2 = QPointF(p2_x - p0_x, p2_y - p0_y);

			box->disable();
			v->restoreUpdateRate();

			// set information of selected division points.
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
				QPointF targetP = QPointF(target_x, target_y);
				selectCtrlZone(targetP, graphicsView()->stdRadius(iRIC::nearRadius()));
			}

			setActionStatus();
			updateShapeData();
			renderGraphicsView();
		}
	} else if (event->button() == Qt::RightButton) {
		if (iRIC::isNear(m_dragStartPoint, event->pos())) {
			m_rightClickingMenu->move(event->globalPos());
			m_rightClickingMenu->show();
		}
	}
}

void GridCreatingConditionRiverSurvey15D::updateMouseCursor(PreProcessorGraphicsViewInterface* /*v*/)
{}

void GridCreatingConditionRiverSurvey15D::doLoadFromProjectMainFile(const QDomNode& /*node*/) {}

void GridCreatingConditionRiverSurvey15D::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/) {}

void GridCreatingConditionRiverSurvey15D::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(3);
}

void GridCreatingConditionRiverSurvey15D::assignActorZValues(const ZDepthRange& range)
{
	double paintDepth = range.min();
	double lineDepth = range.min() * 0.9 + range.max() * 0.1;
	double bigpointDepth = (range.min() + range.max()) * 0.5;
	double pointDepth = range.min() * 0.2 + range.max() * 0.8;
	double selectedBigPointDepth = range.min() * 0.1 + range.max() * 0.9;
	double selectedPointDepth = range.min() * 0.05 + range.max() * 0.95;

	m_createRegionActor->SetPosition(0, 0, paintDepth);
	m_selectedCtrlZoneActor->SetPosition(0, 0, paintDepth);

	m_riverCenterLineActor->SetPosition(0, 0, lineDepth);

	m_blackFixedPointsActor->SetPosition(0, 0, bigpointDepth);
	m_blackCtrlPointsActor->SetPosition(0, 0, bigpointDepth);

	m_fixedPointsActor->SetPosition(0, 0, pointDepth);
	m_ctrlPointsActor->SetPosition(0, 0, pointDepth);

	m_blackSelectedCtrlPointsActor->SetPosition(0, 0, selectedBigPointDepth);
	m_selectedCtrlPointsActor->SetPosition(0, 0, selectedPointDepth);
}

bool GridCreatingConditionRiverSurvey15D::addToolBarButtons(QToolBar* /*tb*/)
{
	return true;
}

void GridCreatingConditionRiverSurvey15D::loadExternalData(const QString& /*filename*/)
{}

void GridCreatingConditionRiverSurvey15D::saveExternalData(const QString& /*filename*/) {}

void GridCreatingConditionRiverSurvey15D::setupActions()
{
	PreProcessorGridCreatingConditionDataItemInterface* p = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	m_createAction = p->createAction();
	m_switchAction = p->switchAction();

	m_addAction = new QAction(tr("&Add Division Points..."), this);
	m_addRegionallyAction = new QAction(tr("Add Division Points &Regionally..."), this);
	m_moveAction = new QAction(tr("&Move Division Points..."), this);
	m_repositionAction = new QAction(tr("Re&position Division Points..."), this);
	m_deleteAction = new QAction(tr("&Delete Division Points..."), this);

	connect(m_addAction, SIGNAL(triggered()), this, SLOT(addPoints()));
	connect(m_addRegionallyAction, SIGNAL(triggered()), this, SLOT(addRegionPoints()));
	connect(m_moveAction, SIGNAL(triggered()), this, SLOT(movePoints()));
	connect(m_repositionAction, SIGNAL(triggered()), this, SLOT(repositionPoints()));
	connect(m_deleteAction, SIGNAL(triggered()), this, SLOT(deletePoints()));
}

void GridCreatingConditionRiverSurvey15D::setActionStatus()
{
	// menu status when zone and points are not selected.
	if ((m_selectedZone.point ==0) && (m_selectedCtrlPointInfoList.size() == 0)) {

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
		if (m_selectedCtrlPointInfoList.size() > 0) {
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

void GridCreatingConditionRiverSurvey15D::addRegionPoints()
{
	GridCreatingConditionRiverSurvey15DPointRegionAddDialog* dialog = new GridCreatingConditionRiverSurvey15DPointRegionAddDialog(this, preProcessorWindow());
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

void GridCreatingConditionRiverSurvey15D::addPoints()
{
	GridCreatingConditionRiverSurvey15DPointAddDialog* dlg = new GridCreatingConditionRiverSurvey15DPointAddDialog(this, preProcessorWindow());
	m_mouseEventMode = meAddPointDialog;

	iricMainWindow()->enterModelessDialogMode();
	dlg->show();
	connect(dlg, SIGNAL(destroyed()), iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dlg, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
}

void GridCreatingConditionRiverSurvey15D::movePoints()
{
	GridCreatingConditionRiverSurvey15DPointMoveDialog* dialog =
		new GridCreatingConditionRiverSurvey15DPointMoveDialog(
		this, m_GCPOffsetInfo.lowerLimit, m_GCPOffsetInfo.upperLimit, preProcessorWindow());
	m_mouseEventMode = meMovePointDialog;

	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GridCreatingConditionRiverSurvey15D::repositionPoints()
{
	GridCreatingConditionRiverSurvey15DPointRepositionDialog* dialog =
		new GridCreatingConditionRiverSurvey15DPointRepositionDialog(this, preProcessorWindow());
	m_mouseEventMode = meRepositionPointDialog;

	dataModel()->iricMainWindow()->enterModelessDialogMode();
	connect(dialog, SIGNAL(destroyed()), dataModel()->iricMainWindow(), SLOT(exitModelessDialogMode()));
	connect(dialog, SIGNAL(destroyed()), this, SLOT(restoreMouseEventMode()));
	dialog->show();
}

void GridCreatingConditionRiverSurvey15D::deletePoints()
{
	updateShapeData();

	QMessageBox::StandardButton button = QMessageBox::warning(
		preProcessorWindow(), tr("Confirmation"),
		tr("Are you sure you want to delete these control points?"),
		(QMessageBox::Yes | QMessageBox::No),
		QMessageBox::No);

	if (button == QMessageBox::Yes) {
		iRICUndoStack::instance().push(new GridCreatingConditionCtrlPointDeleteCommand15D(this));
	}

	invalidateSelectedCtrlPoints();
	setActionStatus();
	updateShapeData();
}

bool GridCreatingConditionRiverSurvey15D::ctrlPointSelectRegion(
	const QPointF& p0, const QPointF& v0, const QPointF& v1)
{
	m_selectedCtrlPointInfoList.clear();

	GeoDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	p->SelectCtrlPointsRegion(p0, v0, v1, m_selectedCtrlPointInfoList);

	invalidateSelectedCtrlPoints();
	return m_selectedCtrlPointInfoList.size() > 0;
}

bool GridCreatingConditionRiverSurvey15D::ctrlPointXORSelectRegion(
	const QPointF& p0, const QPointF& v0, const QPointF& v1)
{
	std::list<CtrlPointSelectionInfo> tmplist;
	GeoDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
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
	return m_selectedCtrlPointInfoList.size() > 0;
}

void GridCreatingConditionRiverSurvey15D::updateShapeData()
{
	m_centerFixedPoints->Reset();       // fixed points on center line.
	m_ctrlPointPoints->Reset();         // control points.
	m_selectedCtrlPointPoints->Reset(); // selected control points.

	m_riverCenterLine->Initialize();

	vtkSmartPointer<vtkPoints> centerLinePoints = vtkSmartPointer<vtkPoints>::New();

	// calculate the number of grid size of m_riverCenterLine etc.
	int pointCount = 0;
	GeoDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	while (p != nullptr) {
		++pointCount;
		p = p->nextPoint();
	}
	int gridSize = pointCount + (pointCount - 1) * (LINEDIVS - 1);
	m_riverCenterLine->SetDimensions(gridSize, 1, 1);

	double point[3];
	point[2] = 0;

	// create fixed points on the center line
	vtkVertex* vtx;
	int index = 0;
	p = m_riverSurvey->headPoint()->nextPoint();
	while (p != nullptr) {
		point[0] = p->position().x();
		point[1] = p->position().y();
		m_centerFixedPoints->InsertNextPoint(point);

		vtx = vtkVertex::New();
		vtx->GetPointIds()->SetId(0, index);
		m_fixedPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
		vtx->Delete();

		++index;
		p = p->nextPoint();
	}

	// create the center line and control points on it
	m_ctrlPoints->Reset();
	int ctrlPointIndex = 0;
	p = m_riverSurvey->headPoint()->nextPoint();
	while (p != nullptr) {
		// control points
		if (p->nextPoint() != nullptr) {
			for (auto cit = p->CenterLineCtrlPoints.begin(); cit != p->CenterLineCtrlPoints.end(); ++cit) {
				QPointF vec = p->CtrlPointPosition2D(GeoDataRiverPathPoint::pposCenterLine, *cit);
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

		// river center line
		QPointF tmpp;
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
		p = p->nextPoint();
	}

	// selected zone
	m_selectedCtrlZone->Initialize();
	if (m_selectedZone.point != nullptr) {
		// zone is selected!
		GeoDataRiverPathPoint* p = m_selectedZone.point;
		QList<QPointF> pointlist = p->CtrlZonePoints(m_selectedZone.position, m_selectedZone.index, ZONEDIV);
		m_selectedCtrlZone->SetDimensions(pointlist.count(), 1, 1);
		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		points->SetDataTypeToDouble();
		for (auto it = pointlist.begin(); it != pointlist.end(); ++it) {
			QPointF v = (*it);
			points->InsertNextPoint(v.x(), v.y(), 0);
		}
		m_selectedCtrlZone->SetPoints(points);
		m_selectedCtrlZone->Modified();
	}

	// selected control points
	m_selectedCtrlPointPoints->Reset();
	m_selectedCtrlPoints->Reset();

	int selectedCtrlPointIndex = 0;
	for (auto it = m_selectedCtrlPointInfoList.begin(); it != m_selectedCtrlPointInfoList.end(); ++it) {
		point[0] = it->Point->CtrlPointPosition2D(it->Position, it->Index).x();
		point[1] = it->Point->CtrlPointPosition2D(it->Position, it->Index).y();
		m_selectedCtrlPointPoints->InsertNextPoint(point);

		vtx = vtkVertex::New();
		vtx->GetPointIds()->SetId(0, selectedCtrlPointIndex);
		m_selectedCtrlPoints->InsertNextCell(vtx->GetCellType(), vtx->GetPointIds());
		vtx->Delete();

		++selectedCtrlPointIndex;
	}

	m_centerFixedPoints->Modified();
	m_fixedPoints->SetPoints(m_centerFixedPoints);
	m_fixedPoints->Modified();

	m_ctrlPointPoints->Modified();
	m_ctrlPoints->SetPoints(m_ctrlPointPoints);
	m_ctrlPoints->Modified();

	m_riverCenterLine->SetPoints(centerLinePoints);
	m_riverCenterLine->Modified();

	m_selectedCtrlPointPoints->Modified();
	m_selectedCtrlPoints->SetPoints(m_selectedCtrlPointPoints);
	m_selectedCtrlPoints->Modified();
}

void GridCreatingConditionRiverSurvey15D::hideCreateRegion()
{
	m_createRegionActor->VisibilityOff();
}

void GridCreatingConditionRiverSurvey15D::createGrid(GeoDataRiverPathPoint* start, GeoDataRiverPathPoint* end, int dataNum)
{
	m_lastStartPoint = start;
	m_lastEndPoint = end;

	Structured15DGridWithCrossSection* grid = new Structured15DGridWithCrossSection(0);
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(m_conditionDataItem->parent()->parent());
	gt->gridType()->buildGridAttributes(grid);

	GeoDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();

	// create grid.
	p = m_lastStartPoint;
	double point[3];
	point[2] = 0;
	while (p != m_lastEndPoint) {
		point[0] = p->position().x();
		point[1] = p->position().y();
		points->InsertNextPoint(point);

		for (auto it = p->CenterLineCtrlPoints.begin(); it != p->CenterLineCtrlPoints.end(); ++it) {
			QPointF v = p->CtrlPointPosition2D(GeoDataRiverPathPoint::pposCenterLine, *it);
			point[0] = v.x();
			point[1] = v.y();
			points->InsertNextPoint(point);
		}

		p = p->nextPoint();
	}
	points->InsertNextPoint(p->position().x(), p->position().y(), 0);
	grid->setPoints(points);
	grid->setModified();

	setupCrosssections(grid);

	// grid related conditions
	QList<GridAttributeContainer*>& clist = grid->gridAttributes();
	for (auto it = clist.begin(); it != clist.end(); ++it) {
		(*it)->allocate();
	}

	// cross sections
	p = m_lastStartPoint;
	int index = 1;
	while (p != m_lastEndPoint->nextPoint()) {
		// add the cross section of a fixed point
		appendCrossSectionToGrid(p->crosssection(), grid, p->name());

		// add cross sections of control points
		processCtrlPoints(&index, grid, p, dataNum);

		p = p->nextPoint();
	}

	emit gridCreated(grid);
	iRICUndoStack::instance().clear();
}

void GridCreatingConditionRiverSurvey15D::processCtrlPoints(int* index, Grid* grid, GeoDataRiverPathPoint* p, int dataNum)
{
	for (auto it = p->CenterLineCtrlPoints.begin(); it != p->CenterLineCtrlPoints.end(); ++it) {
		double ratio = *it;
		int numPoints = (dataNum - 1) / 2;      // dataNum should be odd.

		// create cross section.
		GeoDataRiverCrosssection crosssection;
		LinearInterpolator1D1 interpolator;
		GeoDataRiverPathPoint* fromP = p;
		GeoDataRiverPathPoint* toP = p->nextPoint();
		QPointF ctrlPoint = p->CtrlPointPosition2D(GeoDataRiverPathPoint::pposCenterLine, ratio);

		//center
		double tmpdbl1, tmpdbl2;
		tmpdbl1 = fromP->lXSec()->interpolate(0).height();
		tmpdbl2 = toP->lXSec()->interpolate(0).height();
		interpolator.setValues(tmpdbl1, tmpdbl2);
		crosssection.addPoint(0, interpolator.interpolate(ratio));

		// left bank.
		QPointF leftbank = p->leftBank()->interpolate(ratio);
		double leftbankdist = iRIC::length(leftbank - ctrlPoint);
		for (int i = 1; i <= numPoints; ++i) {
			double tmpt = i / static_cast<double>(numPoints);
			tmpdbl1 = fromP->lXSec()->interpolate(tmpt).height();
			tmpdbl2 = toP->lXSec()->interpolate(tmpt).height();
			interpolator.setValues(tmpdbl1, tmpdbl2);
			crosssection.addPoint(- leftbankdist * tmpt, interpolator.interpolate(ratio));
		}
		//right bank.
		QPointF rightbank = p->rightBank()->interpolate(ratio);
		double rightbankdist = iRIC::length(rightbank - ctrlPoint);
		for (int i = 1; i <= numPoints; ++i) {
			double tmpt = i / static_cast<double>(numPoints);
			tmpdbl1 = fromP->rXSec()->interpolate(tmpt).height();
			tmpdbl2 = toP->rXSec()->interpolate(tmpt).height();
			interpolator.setValues(tmpdbl1, tmpdbl2);
			crosssection.addPoint(rightbankdist * tmpt, interpolator.interpolate(ratio));
		}

		QString name = tr("point%1").arg((*index)++);
		appendCrossSectionToGrid(crosssection, grid, name);
	}
}

void GridCreatingConditionRiverSurvey15D::appendCrossSectionToGrid(GeoDataRiverCrosssection& cs, Grid* grid, const QString& name)
{
	Structured15DGridWithCrossSectionCrossSection* crosssection = new Structured15DGridWithCrossSectionCrossSection(name, grid);
	GeoDataRiverCrosssection::AltitudeList& alist = cs.AltitudeInfo();
	double offset = alist.front().position();
	for (auto it = alist.begin(); it != alist.end(); ++it) {
		Structured15DGridWithCrossSectionCrossSection::Altitude alt;
		alt.m_position = (*it).position() - offset;
		alt.m_height = (*it).height();
		crosssection->altitudeInfo().append(alt);
	}
	dynamic_cast<Structured15DGridWithCrossSection*>(grid)->crossSections().append(crosssection);
}

void GridCreatingConditionRiverSurvey15D::selectCreateRegion(GeoDataRiverPathPoint* start, GeoDataRiverPathPoint* end)
{
	m_createRegion->RemoveAllInputs();
	GeoDataRiverPathPoint* p = start;
	vtkSmartPointer<vtkPoints> createRegionPoints = vtkSmartPointer<vtkPoints>::New();
	createRegionPoints->SetDataTypeToDouble();

	QPointF tmpp;
	double point[3];
	point[0] = p->position().x();
	point[1] = p->position().y();
	point[2] = 0;
	createRegionPoints->InsertNextPoint(point);
	while (p != end && p->nextPoint() != nullptr) {
		for (int i = 1; i < LINEDIVS; ++i) {
			tmpp = p->riverCenter()->interpolate(i / static_cast<double>(LINEDIVS));
			point[0] = tmpp.x();
			point[1] = tmpp.y();
			createRegionPoints->InsertNextPoint(point);
		}
		createRegionPoints->InsertNextPoint(p->nextPoint()->position().x(), p->nextPoint()->position().y(), 0);
		p = p->nextPoint();
	}

	vtkSmartPointer<vtkStructuredGrid> createRegionLine = vtkSmartPointer<vtkStructuredGrid>::New();
	createRegionLine->SetDimensions(createRegionPoints->GetNumberOfPoints(), 1, 1);
	createRegionLine->SetPoints(createRegionPoints);
	vtkSmartPointer<vtkStructuredGridGeometryFilter> filter = vtkSmartPointer<vtkStructuredGridGeometryFilter>::New();
	filter->SetInputData(createRegionLine);
	m_createRegion->AddInputConnection(filter->GetOutputPort());
	renderGraphicsView();
}

void GridCreatingConditionRiverSurvey15D::allActorsOff()
{
	m_riverCenterLineActor->VisibilityOff();

	m_fixedPointsActor->VisibilityOff();
	m_blackFixedPointsActor->VisibilityOff();
	m_blackCtrlPointsActor->VisibilityOff();
	m_ctrlPointsActor->VisibilityOff();

	m_selectedCtrlPointsActor->VisibilityOff();
	m_blackSelectedCtrlPointsActor->VisibilityOff();

	m_createRegionActor->VisibilityOff();
}

void GridCreatingConditionRiverSurvey15D::selectCtrlZone(const QPointF& point, double width)
{
	m_selectedZone.point = nullptr;
	GeoDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
	double rwidth = width;
	bool found = false;
	bool region;
	while (! found && p != nullptr) {
		if (p->gridSkip()) {
			p = p->nextPoint();
			continue;
		}
		QPointF mins, maxs;
		p->centerToBanksRegion(mins, maxs);
		double xwidth = maxs.x() - mins.x();
		double ywidth = maxs.y() - mins.y();
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
				found = found || selectCtrlZone(p, GeoDataRiverPathPoint::zposCenterLine, point, rwidth);
			}
		}
		p = p->nextPoint();
	}
}

void GridCreatingConditionRiverSurvey15D::setupCrosssections(Grid* grid)
{
	// if the solver has attribute "Crosssection", and it is a complex type,
	// create groups.

	auto csGroup = gccDataItem()->gridTypeDataItem()->geoDataTop()->groupDataItem("Crosssection");
	if (csGroup == nullptr) {return;}

	auto csComplexGroup = dynamic_cast<PreProcessorGeoDataComplexGroupDataItemInterface*> (csGroup);
	if (csComplexGroup == nullptr) {return;}

	auto cAttr = dynamic_cast<SolverDefinitionGridComplexAttribute*> (csComplexGroup->condition());
	if (cAttr == nullptr && cAttr->isGrouped()) {return;}

	csComplexGroup->setupGroups(grid->nodeCount());

	auto groups = csComplexGroup->groups();

	auto point = m_riverSurvey->headPoint()->nextPoint();

	for (int i = 0; i < groups.size(); ++i) {
		auto g = groups[i];
		auto cs = g->containerSet();

		auto ai = point->crosssection().AltitudeInfo();
		std::vector<double> x, y, zeros;
		for (int j = 0; j < ai.size(); ++j) {
			auto alt = ai.at(j);
			if (! alt.active()) {continue;}
			x.push_back(alt.position());
			y.push_back(alt.height());
			zeros.push_back(0);
		}

		if (cs->container("name") != nullptr) {
			cs->string("name").setValue(point->name());
		}

		if (cs->container("appr_xs") != nullptr) {
			cs->functional("appr_xs").setValue(x, y);
		}

		if (cs->container("n_h") != nullptr) {
			auto& n_h = cs->functional("n_h");
			n_h.param() = x;
			n_h.value("Y") = y;
			n_h.value("NVAL") = zeros;

			auto& n_h_v = cs->functional("n_h_v");
			n_h_v.param() = x;
			n_h_v.value("Y") = y;
			n_h_v.value("BOTD") = zeros;
			n_h_v.value("TOPD") = zeros;
			n_h_v.value("BOTN") = zeros;
			n_h_v.value("TOPN") = zeros;
		}

		point = point->nextPoint();
	}
}

bool GridCreatingConditionRiverSurvey15D::selectCtrlZone(GeoDataRiverPathPoint* p, GeoDataRiverPathPoint::CtrlZonePosition pos, const QPointF& point, double width)
{
	bool found = false;
	unsigned int maxindex = static_cast<unsigned int>(p->CtrlPoints(pos).size());
	for (unsigned int i = 0; ! found && i <= maxindex; ++i) {
		QList<QPointF> vectorlist = p->CtrlZonePoints(pos, i, ZONEDIV);
		QPointF v0, v1;
		auto it = vectorlist.begin();
		v0 = *it++;
		while (! found && it != vectorlist.end()) {
			v1 = *it++;
			QPointF horizontal = v1 - v0;
			QPointF vertical = iRIC::normalize((v1 - v0));
			iRIC::rotateVector90(vertical);
			vertical *= width;
			QPointF posv = v0 - vertical * 0.5;
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

void GridCreatingConditionRiverSurvey15D::restoreMouseEventMode()
{
	m_mouseEventMode = meNormal;
}

bool GridCreatingConditionRiverSurvey15D::isCtrlPointsContinuousSelection()
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

void GridCreatingConditionRiverSurvey15D::invalidateSelectedCtrlPoints()
{
	std::list<CtrlPointSelectionInfo> tmplist;
	for (auto it = m_selectedCtrlPointInfoList.begin();
			 it != m_selectedCtrlPointInfoList.end();
			 ++it) {
		if (it->Index < it->Point->CtrlPoints(it->Position).size()) {
			// Valid
			tmplist.push_back(*it);
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
			m_GCPOffsetInfo.direction = iRIC::normalize(
					info.Point->CtrlPointPosition2D(info.Position, d) -
					info.Point->CtrlPointPosition2D(info.Position, d - 0.01));
		} else {
			m_GCPOffsetInfo.direction = iRIC::normalize(
					info.Point->CtrlPointPosition2D(info.Position, d + 0.01) -
					info.Point->CtrlPointPosition2D(info.Position, d));
		}
		if (info.Position == GeoDataRiverPathPoint::pposCenterToLeft) {
			m_GCPOffsetInfo.length = std::abs(info.Point->crosssection().leftBank(true).position());
		} else if (info.Position == GeoDataRiverPathPoint::pposCenterToRight) {
			m_GCPOffsetInfo.length = std::abs(info.Point->crosssection().rightBank(true).position());
		} else {
			m_GCPOffsetInfo.length = iRIC::length(info.Point->nextPoint()->position() - info.Point->position());
		}
	}
}

bool GridCreatingConditionRiverSurvey15D::checkCtrlPointsRegion(GeoDataRiverPathPoint* start, GeoDataRiverPathPoint* end)
{
	bool ans = false;

	GeoDataRiverPathPoint* tmpp;

	// Check whether data already exist in the region
	tmpp = start;
	while (tmpp != end) {
		if (tmpp->CenterLineCtrlPoints.size() > 0) {
			ans = true;
		}
		tmpp = tmpp->nextPoint();
	}

	return ans;
}

void GridCreatingConditionRiverSurvey15D::handleDataChange()
{
	updateShapeData();
}

void GridCreatingConditionRiverSurvey15D::handleDataDestroy()
{
	// River survey data is destroyed.
	// This algorithm is no longer available.
	PreProcessorGridCreatingConditionDataItemInterface* item = dynamic_cast<PreProcessorGridCreatingConditionDataItemInterface*>(parent());
	item->setCondition(0);
	delete this;
}

void GridCreatingConditionRiverSurvey15D::clear()
{
	GeoDataRiverPathPoint* p = m_riverSurvey->headPoint()->nextPoint();
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
	updateShapeData();
}
