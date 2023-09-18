#include "geodatapointmap_pointsmanager_addpointcontroller.h"
#include "geodatapointmap_pointsmanager_addpointcontroller_setexistingpointcommand.h"
#include "geodatapointmap_pointsmanager_addpointscommand.h"
#include "geodatapointmap_pointsmanager_selectpointboundingboxcontroller.h"

#include <guibase/polyline/polylinepushvertexcommand.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/pre/base/preprocessordatamodeli.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>

#include <QMessageBox>

GeoDataPointmap::PointsManager::AddPointController::AddPointController(PointsManager* manager) :
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_pointsController {},
	m_existingPointId {-1},
	m_existingPointValue {0},
	m_mouseEventMode {MouseEventMode::SelectExistingPointNotPossible},
	m_manager {manager}
{
	auto v = manager->m_parent->dataModel()->graphicsView();
	m_pointsController.pointsActor()->GetProperty()->SetPointSize(7 * v->devicePixelRatioF());
}

GeoDataPointmap::PointsManager::AddPointController::~AddPointController()
{}

vtkActor* GeoDataPointmap::PointsManager::AddPointController::pointsActor() const
{
	return m_pointsController.pointsActor();
}

void GeoDataPointmap::PointsManager::AddPointController::setZDepthForPoints(double z)
{
	m_pointsController.pointsActor()->SetPosition(0, 0, z);
}

void GeoDataPointmap::PointsManager::AddPointController::addActorsToRenderer(vtkRenderer* renderer)
{
	renderer->AddActor(m_pointsController.pointsActor());
}

void GeoDataPointmap::PointsManager::AddPointController::removeActorsFromRenderer(vtkRenderer* renderer)
{
	renderer->RemoveActor(m_pointsController.pointsActor());
}

void GeoDataPointmap::PointsManager::AddPointController::activate(VTK2DGraphicsView* /*v*/)
{
	auto w = m_manager->m_parent->preProcessorWindow();
	InformationDialog::information(w, GeoDataPointmap::tr("Information"), GeoDataPointmap::tr("To add new points, select an existing point first. The value at that point will be used as the "
																 "default value for new points. Then, click at any position you want to add points, and finish by double-clicking of pressing return key."), "geodatapointmapaddpoint");

	m_mouseEventMode = MouseEventMode::SelectExistingPointNotPossible;
	m_pointsController.clear();

	m_manager->m_parent->actorCollection()->AddItem(m_pointsController.pointsActor());
	m_manager->m_parent->updateVisibilityWithoutRendering();
}

void GeoDataPointmap::PointsManager::AddPointController::deactivate(VTK2DGraphicsView* /*v*/)
{
	m_mouseEventMode = MouseEventMode::SelectExistingPointNotPossible;
	m_pointsController.clear();

	m_pointsController.pointsActor()->VisibilityOff();
	m_manager->m_parent->actorCollection()->RemoveItem(m_pointsController.pointsActor());
}

void GeoDataPointmap::PointsManager::AddPointController::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* /*v*/)
{
	if (iRIC::isEnterKey(event->key())) {
		finish();
	}
}

void GeoDataPointmap::PointsManager::AddPointController::handleMouseDoubleClickEvent(QMouseEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{
	finish();
}

void GeoDataPointmap::PointsManager::AddPointController::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (m_mouseEventMode == MouseEventMode::SelectExistingPointNotPossible || m_mouseEventMode == MouseEventMode::SelectExistingPointPossible) {
		auto worldPos = v->viewportToWorld(event->pos());
		double radius = v->stdRadius(iRIC::nearRadius());
		m_existingPointId = m_manager->closestPointWithinRadius(worldPos, radius);
		if (m_existingPointId >= 0) {
			m_existingPointValue = m_manager->m_points->GetPointData()->GetArray(VALUES)->GetTuple1(m_existingPointId);
			m_mouseEventMode = MouseEventMode::SelectExistingPointPossible;
		} else {
			m_mouseEventMode = MouseEventMode::SelectExistingPointNotPossible;
		}
		updateMouseCursor(v);
	}
}

void GeoDataPointmap::PointsManager::AddPointController::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (m_mouseEventMode == MouseEventMode::SelectExistingPointPossible) {
		m_manager->m_parent->pushRenderCommand(new SetExistingPointCommand(m_existingPointId, m_existingPointValue, this));
	} else if (m_mouseEventMode == MouseEventMode::AddPoint) {
		QPointF point = v->viewportToWorld(event->pos());
		m_manager->m_parent->pushRenderCommand(new PolyLinePushVertexCommand(true, point, &m_pointsController));
	}
}

void GeoDataPointmap::PointsManager::AddPointController::updateMouseCursor(VTK2DGraphicsView* v)
{
	if (m_mouseEventMode == MouseEventMode::SelectExistingPointNotPossible) {
		v->setCursor(Qt::ArrowCursor);
	} else if (m_mouseEventMode == MouseEventMode::SelectExistingPointPossible) {
		v->setCursor(m_addCursor);
	} else if (m_mouseEventMode == MouseEventMode::AddPoint) {
		v->setCursor(Qt::CrossCursor);
	}
}

void GeoDataPointmap::PointsManager::AddPointController::finish()
{
	auto w = m_manager->m_parent->preProcessorWindow();
	if (m_pointsController.polyLine().size() < 2) {
		int ret = QMessageBox::information(w, GeoDataPointmap::tr("Warning"),
																			 GeoDataPointmap::tr("You must select one known point, and define at least one location for added points. "
																			"Do you want to retry, or cancel the operation?"), QMessageBox::Retry | QMessageBox::Cancel, QMessageBox::Cancel);
		if (ret == QMessageBox::Retry) {return;}
	} else {
		bool ok;
		double val = QInputDialog::getDouble(w, GeoDataPointmap::tr("Add Points"), GeoDataPointmap::tr("Values of new points"), m_existingPointValue, -10000, 10000, 3, &ok);
		if (ok) {
			auto newPoints = m_pointsController.polyLine();
			newPoints.erase(newPoints.begin()); // first point is the existing point

			std::vector<double> vals;
			for (unsigned int i = 0; i < newPoints.size(); ++i) {
				vals.push_back(val);
			}
			m_pointsController.clear();
			iRICUndoStack::instance().clear(); // to avoid crash while undoing
			m_manager->m_parent->pushModifyCommand(new AddPointsCommand(newPoints, vals, m_manager));
		}
	}
	m_manager->switchToSelectPointBoundingBoxMode();
}
