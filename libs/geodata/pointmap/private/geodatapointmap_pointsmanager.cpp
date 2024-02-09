#include "geodatapointmap_impl.h"
#include "geodatapointmap_pointsmanager.h"
#include "geodatapointmap_pointsmanager_actions.h"
#include "geodatapointmap_pointsmanager_addpointcontroller.h"
#include "geodatapointmap_pointsmanager_editpointsdialog.h"
#include "geodatapointmap_pointsmanager_deletepointscommand.h"
#include "geodatapointmap_pointsmanager_filterselectedpointsdialog.h"
#include "geodatapointmap_pointsmanager_interpolatepointcontroller.h"
#include "geodatapointmap_pointsmanager_selectpointboundingboxcontroller.h"
#include "geodatapointmap_pointsmanager_selectpointpolygoncontroller.h"
#include "geodatapointmap_tinmanager.h"

#include <guibase/vtktool/vtkpolydatamapperutil.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <misc/lastiodirectory.h>
#include <misc/informationdialog.h>
#include <misc/mathsupport.h>
#include <misc/tpoexporter.h>

#include <vtkActor.h>
#include <vtkLODActor.h>
#include <vtkPointLocator.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>

#include <algorithm>

namespace {

int SELECTED_POINT_SIZE = 9;

vtkCellArray* buildCells(const std::unordered_set<vtkIdType>& indices)
{
	std::vector<vtkIdType> ivec;
	ivec.reserve(indices.size());
	for (vtkIdType index : indices) {
		ivec.push_back(index);
	}
	std::sort(ivec.begin(), ivec.end());

	auto ret = vtkCellArray::New();
	ret->Allocate(ivec.size(), 10);
	for (vtkIdType index : ivec) {
		ret->InsertNextCell(1, &index);
	}
	return ret;
}

} // namespace

GeoDataPointmap::PointsManager::PointsManager(GeoDataPointmap* pointmap) :
	m_points {vtkPolyData::New()},
	m_pointsActor {vtkLODActor::New()},
	m_maskPoints10k {vtkMaskPoints::New()},
	m_maskPoints40k {vtkMaskPoints::New()},
	m_selectedPoints {vtkPolyData::New()},
	m_selectedPointsActor {vtkActor::New()},
	m_pointLocator {vtkPointLocator::New()},
	m_tinManager {nullptr},
	m_parent {pointmap},
	m_selectPointBoundingBoxController {new SelectPointBoundingBoxController {this}},
	m_selectPointPolygonController {new SelectPointPolygonController {this}},
	m_addPointController {new AddPointController {this}},
	m_interpolatePointController {new InterpolatePointController {this}},
	m_activeController {m_selectPointBoundingBoxController},
	m_actions {new Actions {this}}
{
	m_controllers.push_back(m_selectPointBoundingBoxController);
	m_controllers.push_back(m_selectPointPolygonController);
	m_controllers.push_back(m_addPointController);
	m_controllers.push_back(m_interpolatePointController);

	setupSettings();
	updateActionStatus();
}

GeoDataPointmap::PointsManager::~PointsManager()
{
	m_points->Delete();
	m_pointsActor->Delete();
	m_maskPoints10k->Delete();
	m_maskPoints40k->Delete();
	m_selectedPoints->Delete();
	m_selectedPointsActor->Delete();
	m_pointLocator->Delete();

	delete m_selectPointBoundingBoxController;
	delete m_selectPointPolygonController;
	delete m_addPointController;
	delete m_interpolatePointController;
	delete m_actions;
}

void GeoDataPointmap::PointsManager::setZDepthForAddAndInterpolatePoints(double z)
{
	m_addPointController->setZDepthForPoints(z);
	m_interpolatePointController->setZDepthForPoints(z);
}

vtkIdType GeoDataPointmap::PointsManager::closestPointWithinRadius(const QPointF& point, double radius)
{
	double p[3] = {point.x(), point.y(), 0.0};
	vtkIdType id = m_pointLocator->FindClosestPoint(p);
	if (id < 0) {return -1;}

	double p2[3];
	m_points->GetPoint(id, p2);
	QPointF nearest(p2[0], p2[1]);

	double dist = iRIC::distance(point, nearest);
	if (dist > radius) {return -1;}

	return id;
}

std::vector<vtkIdType> GeoDataPointmap::PointsManager::pointsWithinRadius(const QPointF& point, double radius)
{
	double p[3] = {point.x(), point.y(), 0.0};
	auto idlist = vtkSmartPointer<vtkIdList>::New();
	m_pointLocator->FindPointsWithinRadius(radius, p, idlist);

	std::vector<vtkIdType> ids;
	for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
		ids.push_back(idlist->GetId(i));
	}

	return ids;
}

void GeoDataPointmap::PointsManager::setTinManager(TINManager* manager)
{
	m_tinManager = manager;
}

void GeoDataPointmap::PointsManager::setSelectionPolygonZDepth(double z)
{
	m_selectPointPolygonController->setSelectionPolygonZDepth(z);
}

void GeoDataPointmap::PointsManager::setSelectedPointsSize(double size)
{
	m_selectedPointsActor->GetProperty()->SetPointSize(size);
	m_addPointController->pointsActor()->GetProperty()->SetPointSize(size);
	m_interpolatePointController->pointsActor()->GetProperty()->SetPointSize(size);
}

void GeoDataPointmap::PointsManager::setPoints(vtkPoints* points, vtkDoubleArray* values)
{
	m_points->Initialize();
	m_points->GetPointData()->Initialize();

	m_points->SetPoints(points);
	values->SetName(VALUES);
	m_points->GetPointData()->AddArray(values);
	m_points->GetPointData()->SetActiveScalars(VALUES);

	// setup verts
	auto verts = vtkSmartPointer<vtkCellArray>::New();
	for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
		verts->InsertNextCell(1, &i);
	}
	m_points->SetVerts(verts);
	m_points->BuildCells();
}

std::vector<vtkIdType> GeoDataPointmap::PointsManager::selectedPointIndices() const
{
	auto unordered_set = selectedPointIndicesUnorderedSet();
	std::vector<vtkIdType> ret;
	ret.reserve(unordered_set.size());

	for (auto v : unordered_set) {
		ret.push_back(v);
	}
	std::sort(ret.begin(), ret.end());
	return ret;
}

std::unordered_set<vtkIdType> GeoDataPointmap::PointsManager::selectedPointIndicesUnorderedSet() const
{
	std::unordered_set<vtkIdType> ret;

	auto verts = m_selectedPoints->GetVerts();
	auto ids = vtkSmartPointer<vtkIdList>::New();
	verts->InitTraversal();
	for (vtkIdType i = 0; i < verts->GetNumberOfCells(); ++i) {
		verts->GetNextCell(ids);
		vtkIdType pointId = ids->GetId(0);
		ret.insert(pointId);
	}
	return ret;
}

void GeoDataPointmap::PointsManager::setSelectedPointIndices(const std::unordered_set<vtkIdType>& indices)
{
	auto cells = buildCells(indices);
	m_selectedPoints->SetVerts(cells);
	m_selectedPoints->Modified();
	cells->Delete();
}

void GeoDataPointmap::PointsManager::selectPointsInsideBox(MouseBoundingBox* box, bool xOr)
{
	QPointF c = box->center();
	double radius = box->circumfenceCircleRadius() * 1.1;
	double center[3];
	center[0] = c.x();
	center[1] = c.y();
	center[2] = 0;
	auto idlist = vtkSmartPointer<vtkIdList>::New();
	m_pointLocator->FindPointsWithinRadius(radius, center, idlist);

	std::unordered_set<vtkIdType> selectedIndices;
	if (xOr) {
		selectedIndices = selectedPointIndicesUnorderedSet();
	}

	clearSelection();

	for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); i++) {
		double p[3];
		m_points->GetPoint(idlist->GetId(i), p);
		if (box->isInsideBox(p[0], p[1])) {
			// this is inside the box!
			vtkIdType pointId = idlist->GetId(i);
			auto it = selectedIndices.find(pointId);
			if (it != selectedIndices.end()) {
				selectedIndices.erase(it);
			} else {
				selectedIndices.insert(pointId);
			}
		}
	}

	auto cells = buildCells(selectedIndices);
	m_selectedPoints->SetVerts(cells);
	m_selectedPoints->Modified();
	cells->Delete();

	updateActionStatus();
}

void GeoDataPointmap::PointsManager::selectPointsInsidePolygon(const QPolygonF& polygon, bool xOr)
{
	std::unordered_set<vtkIdType> selectedIndices;
	if (xOr) {
		selectedIndices = selectedPointIndicesUnorderedSet();
	}

	clearSelection();

	auto rect = polygon.boundingRect();
	auto c = rect.center();
	auto radius = std::sqrt(rect.width() * rect.width() + rect.height() * rect.height()) / 2;

	double center[3];
	center[0] = c.x(); center[1] = c.y(); center[2] = 0;
	auto idlist = vtkSmartPointer<vtkIdList>::New();

	m_pointLocator->FindPointsWithinRadius(radius, center, idlist);

	for (vtkIdType i = 0; i < idlist->GetNumberOfIds(); ++i) {
		vtkIdType pointId = idlist->GetId(i);
		double x[3];
		m_points->GetPoint(pointId, x);
		QPointF p(x[0], x[1]);
		bool inside = polygon.containsPoint(p, Qt::OddEvenFill);
		if (! inside) {continue;}

		auto it = selectedIndices.find(pointId);
		if (it != selectedIndices.end()) {
			selectedIndices.erase(it);
		} else {
			selectedIndices.insert(pointId);
		}
	}
	auto cells = buildCells(selectedIndices);
	m_selectedPoints->SetVerts(cells);
	m_selectedPoints->Modified();
	cells->Delete();

	updateActionStatus();
}

void GeoDataPointmap::PointsManager::selectNearestPoint(const QPointF& pos, double radius, bool xOr)
{
	double point[3] = {pos.x(), pos.y(), 0};
	vtkIdType nearestId = m_pointLocator->FindClosestPoint(point);
	double point2[3];
	m_points->GetPoint(nearestId, point2);
	QPointF vertexPos(point2[0], point2[1]);
	double dist = iRIC::distance(vertexPos, pos);

	vtkIdType pointToAdd = -1;

	if (dist < radius) {
		// it is near enough. select this point.
		pointToAdd = nearestId;
	}

	std::unordered_set<vtkIdType> selectedIndices;
	if (xOr) {
		selectedIndices = selectedPointIndicesUnorderedSet();
	}
	clearSelection();

	if (pointToAdd != -1) {
		auto it = selectedIndices.find(pointToAdd);
		if (it != selectedIndices.end()) {
			selectedIndices.erase(it);
		} else {
			selectedIndices.insert(pointToAdd);
		}
	}
	auto cells = buildCells(selectedIndices);
	m_selectedPoints->SetVerts(cells);
	m_selectedPoints->Modified();
	cells->Delete();
}

void GeoDataPointmap::PointsManager::clearSelection()
{
	m_selectedPoints->Initialize();
	m_selectedPoints->SetPoints(m_points->GetPoints());
	m_selectedPoints->Modified();
}

void GeoDataPointmap::PointsManager::addActorsToRenderer(vtkRenderer* renderer)
{
	renderer->AddActor(m_pointsActor);
	renderer->AddActor(m_selectedPointsActor);

	for (auto c : m_controllers) {
		c->addActorsToRenderer(renderer);
	}
}

void GeoDataPointmap::PointsManager::removeActorsFromRenderer(vtkRenderer* renderer)
{
	renderer->RemoveActor(m_pointsActor);
	renderer->RemoveActor2D(m_selectedPointsActor);

	for (auto c : m_controllers) {
		c->removeActorsFromRenderer(renderer);
	}
}

void GeoDataPointmap::PointsManager::addActorsForPointsMode(vtkActorCollection* col)
{
	col->AddItem(m_pointsActor);
	col->AddItem(m_selectedPointsActor);
}

void GeoDataPointmap::PointsManager::addActionsToMenu(QMenu* menu)
{
	menu->addMenu(m_actions->selectionModeMenu);

	menu->addSeparator();

	menu->addAction(m_actions->addPointsAction);
	// menu->addAction(m_actions->interpolatePointsAction);

	menu->addSeparator();

	menu->addAction(m_actions->editSelectedPointsAction);
	menu->addAction(m_actions->deleteSelectedPointsAction);
	menu->addAction(m_actions->filterSelectedPointsAction);
	menu->addAction(m_actions->exportSelectedPointsAction);

	menu->addSeparator();

	menu->addAction(m_actions->remeshTinAction);
}

void GeoDataPointmap::PointsManager::deactivate(VTK2DGraphicsView* v)
{
	clearSelection();
}

void GeoDataPointmap::PointsManager::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleKeyPressEvent(event, v);
}

void GeoDataPointmap::PointsManager::handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleKeyReleaseEvent(event, v);
}

void GeoDataPointmap::PointsManager::handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleMouseDoubleClickEvent(event, v);
}

void GeoDataPointmap::PointsManager::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleMouseMoveEvent(event, v);
}

void GeoDataPointmap::PointsManager::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleMousePressEvent(event, v);
}

void GeoDataPointmap::PointsManager::handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_activeController->handleMouseReleaseEvent(event, v);
}

vtkPolyData* GeoDataPointmap::PointsManager::points() const
{
	return m_points;
}

vtkDoubleArray* GeoDataPointmap::PointsManager::values() const
{
	return vtkDoubleArray::SafeDownCast(m_points->GetPointData()->GetArray(VALUES));
}

vtkLODActor* GeoDataPointmap::PointsManager::pointsActor() const
{
	return m_pointsActor;
}

vtkMaskPoints* GeoDataPointmap::PointsManager::maskPoints10k() const
{
	return m_maskPoints10k;
}

vtkMaskPoints* GeoDataPointmap::PointsManager::maskPoints40k() const
{
	return m_maskPoints40k;
}

vtkPolyData* GeoDataPointmap::PointsManager::selectedPoints() const
{
	return m_selectedPoints;
}

vtkActor* GeoDataPointmap::PointsManager::selectedPointsActor() const
{
	return m_selectedPointsActor;
}

vtkPointLocator* GeoDataPointmap::PointsManager::pointLocator() const
{
	return m_pointLocator;
}

void GeoDataPointmap::PointsManager::toggleRectSelectMode(bool /*on*/)
{
	for (auto a : m_actions->checkableActions) {
		a->setChecked(false);
	}
	auto v = m_parent->dataModel()->graphicsView();
	m_activeController->deactivate(v);
	m_activeController = m_selectPointBoundingBoxController;
	m_activeController->activate(v);
	m_actions->selectionModeRectAction->setChecked(true);
}

void GeoDataPointmap::PointsManager::togglePolygonSelectMode(bool on)
{
	for (auto a : m_actions->checkableActions) {
		a->setChecked(false);
	}
	auto v = m_parent->dataModel()->graphicsView();
	m_activeController->deactivate(v);
	if (on) {
		m_actions->selectionModePolygonAction->setChecked(true);
		m_activeController = m_selectPointPolygonController;
	} else {
		m_actions->selectionModeRectAction->setChecked(true);
		m_activeController = m_selectPointBoundingBoxController;
	}
	m_activeController->activate(v);
}

void GeoDataPointmap::PointsManager::toggleAddPointsMode(bool on)
{
	for (auto a : m_actions->checkableActions) {
		a->setChecked(false);
	}
	auto v = m_parent->dataModel()->graphicsView();
	m_activeController->deactivate(v);
	if (on) {
		clearSelection();
		m_actions->addPointsAction->setChecked(true);
		m_activeController = m_addPointController;
	} else {
		m_actions->selectionModeRectAction->setChecked(true);
		m_activeController = m_selectPointBoundingBoxController;
	}
	m_activeController->activate(v);
	m_parent->renderGraphicsView();
}

/*
void GeoDataPointmap::PointsManager::toggleInterpolatePointsMode(bool on)
{
	for (auto a : m_actions->checkableActions) {
		a->setChecked(false);
	}
	auto v = m_parent->dataModel()->graphicsView();
	m_activeController->deactivate(v);
	if (on) {
		clearSelection();
		m_actions->interpolatePointsAction->setChecked(true);
		m_activeController = m_interpolatePointController;
	} else {
		m_actions->selectionModeRectAction->setChecked(true);
		m_activeController = m_selectPointBoundingBoxController;
	}
	m_activeController->activate(v);
	m_parent->renderGraphicsView();
}
*/

void GeoDataPointmap::PointsManager::editSelectedPoints()
{
	auto w = m_parent->preProcessorWindow();
	EditPointsDialog dialog(this, w);
	dialog.exec();
}

void GeoDataPointmap::PointsManager::deleteSelectedPoints()
{
	if (m_parent->impl->m_tinManager.pointsUsedForBreakLines(selectedPointIndicesUnorderedSet())) {
		QMessageBox::warning(m_parent->preProcessorWindow(), tr("Warning"), tr("You can not delete points used for break lines."));
		return;
	}

	vtkIdType leftPoints = m_points->GetNumberOfPoints() - m_selectedPoints->GetVerts()->GetNumberOfCells();
	if (leftPoints < 3) {
		QMessageBox::critical(m_parent->preProcessorWindow(), tr("Error"), tr("After deleting, the point cloud data will have %1 points. Point cloud data must contain 3 points at least.").arg(leftPoints));
		return;
	}

	auto selected = selectedPointIndicesUnorderedSet();
	clearSelection();
	m_parent->pushModifyCommand(new DeletePointsCommand(selected, this));
}

void GeoDataPointmap::PointsManager::filterSelectedPoints()
{
	auto w = m_parent->preProcessorWindow();
	FilterSelectedPointsDialog dialog(this, w);
	dialog.exec();
}

void GeoDataPointmap::PointsManager::exportSelectedPoints()
{
	auto w = m_parent->preProcessorWindow();
	auto dir = LastIODirectory::get();
	auto fname = QFileDialog::getSaveFileName(w, GeoDataPointmap::tr("Select file name to export"), dir,
																						GeoDataPointmap::tr("Topography File (*.tpo)"));

	if (fname.isNull()) {return;}

	TpoExporter exporter(w);
	bool ok = exporter.open(fname);
	if (! ok) {
		QMessageBox::critical(w, GeoDataPointmap::tr("Error"), GeoDataPointmap::tr("Failed to open %1.").arg(QDir::toNativeSeparators(fname)));
		return;
	}

	auto points2 = points()->GetPoints();
	auto vals = values();
	double v[3];
	for (auto index : selectedPointIndices()) {
		points2->GetPoint(index, v);
		double val = vals->GetValue(index);
		exporter.addPoint(v[0], v[1], val);
	}

	exporter.close();

	QFileInfo finfo(fname);
	LastIODirectory::set(finfo.absolutePath());
}

void GeoDataPointmap::PointsManager::remeshTinAndSwitchToTinEditMode()
{
	InformationDialog::information(m_parent->preProcessorWindow(), GeoDataPointmap::tr("Information"),
																 GeoDataPointmap::tr("Remesh TIN and when it is finished, switch to TIN Edit Mode."), "geodatapointmap_pointsmanager_remeshtin");
	m_parent->remeshTin();
	m_parent->toggleTinEditMode();
}

void GeoDataPointmap::PointsManager::setupSettings()
{
	// m_points
	auto points = vtkPoints::New();
	points->SetDataTypeToDouble();
	m_points->SetPoints(points);
	points->Delete();

	/*
	// m_pointsActor
	m_pointsActor->SetMapper(m_pointsMapper);
	m_pointsActor->AddLODMapper(m_maskPoints40kMapper);
	m_pointsActor->AddLODMapper(m_maskPoints10kMapper);
	*/

	auto p = m_pointsActor->GetProperty();
	p->SetLighting(false);

	// m_delaunayedTrianglesActor
	/*
	m_delaunayedTrianglesActor->SetMapper(m_delaunayedTrianglesMapper);
	m_delaunayedTrianglesActor->AddLODMapper(m_pointsMapper);
	m_delaunayedTrianglesActor->AddLODMapper(m_maskPoints40kMapper);
	m_delaunayedTrianglesActor->AddLODMapper(m_maskPoints10kMapper);
	*/

	/*
	// m_delaunayedTrianglesMapper
	m_delaunayedTrianglesMapper->SetUseLookupTableScalarRange(true);
	m_delaunayedTrianglesMapper->SetScalarVisibility(true);
	*/

	// m_maskPoints10k
	m_maskPoints10k->RandomModeOn();
	m_maskPoints10k->SetMaximumNumberOfPoints(10000);
	m_maskPoints10k->GenerateVerticesOn();
	m_maskPoints10k->SetInputData(m_points);

	// m_maskPoints40k
	m_maskPoints40k->RandomModeOn();
	m_maskPoints40k->SetMaximumNumberOfPoints(40000);
	m_maskPoints40k->GenerateVerticesOn();
	m_maskPoints40k->SetInputData(m_points);

	m_selectedPoints->SetPoints(m_points->GetPoints());

	auto v = m_parent->dataModel()->graphicsView();

	auto prop = m_selectedPointsActor->GetProperty();
	prop->SetPointSize(SELECTED_POINT_SIZE * v->devicePixelRatioF());
	prop->SetLighting(false);
	prop->SetColor(0, 0, 0); // black
	prop->SetRepresentationToPoints();

	auto mapper = vtkPolyDataMapperUtil::createWithScalarVisibilityOff();
	mapper->SetInputData(m_selectedPoints);
	m_selectedPointsActor->SetMapper(mapper);
	mapper->Delete();

	m_pointLocator->SetDataSet(m_points);
}

void GeoDataPointmap::PointsManager::switchToSelectPointBoundingBoxMode()
{
	auto v = m_parent->dataModel()->graphicsView();
	m_activeController->deactivate(v);
	m_activeController = m_selectPointBoundingBoxController;
	m_activeController->activate(v);

	for (auto a : m_actions->checkableActions) {
		a->setChecked(false);
	}
	m_actions->selectionModeRectAction->setChecked(true);
}

void GeoDataPointmap::PointsManager::updateActionStatus()
{
	bool selected = (selectedPointIndicesUnorderedSet().size() > 0);
	m_actions->editSelectedPointsAction->setChecked(selected);
	m_actions->filterSelectedPointsAction->setChecked(selected);
	m_actions->deleteSelectedPointsAction->setChecked(selected);
	m_actions->exportSelectedPointsAction->setChecked(selected);
}
