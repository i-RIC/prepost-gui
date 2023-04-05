#include "geodatapointmap_pointsmanager_selectpointpolygoncontroller.h"

#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guibase/polygon/polygonpushvertexcommand.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/keyboardsupport.h>

#include <QMessageBox>

GeoDataPointmap::PointsManager::SelectPointPolygonController::SelectPointPolygonController(PointsManager* manager) :
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_addMode {false},
	m_polygonController {},
	m_manager {manager}
{
	auto v = manager->m_parent->dataModel()->graphicsView();
	m_polygonController.linesActor()->GetProperty()->SetLineWidth(2 * v->devicePixelRatioF());
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::setSelectionPolygonZDepth(double z)
{
	m_polygonController.linesActor()->SetPosition(0, 0, z);
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::cancelDefiningPolygon()
{
	m_polygonController.clear();
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::addActorsToRenderer(vtkRenderer* renderer)
{
	renderer->AddActor(m_polygonController.linesActor());
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::removeActorsFromRenderer(vtkRenderer* renderer)
{
	renderer->RemoveActor(m_polygonController.linesActor());
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::activate(VTK2DGraphicsView* /*v*/)
{
	m_polygonController.linesActor()->VisibilityOn();
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::deactivate(VTK2DGraphicsView* v)
{
	cancelDefiningPolygon();
	m_addMode = false;

	updateMouseCursor(v);

	m_polygonController.linesActor()->VisibilityOff();
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_addMode = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	updateMouseCursor(v);

	if (iRIC::isEnterKey(event->key())) {
		finishDefiningPolygonAndSelectPoints(false, m_addMode);
	}
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_addMode = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	updateMouseCursor(v);
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::handleMouseDoubleClickEvent(QMouseEvent* event, VTK2DGraphicsView* /*v*/)
{
	if (event->button() != Qt::LeftButton) {return;}

	finishDefiningPolygonAndSelectPoints(true, m_addMode);
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (m_polygonController.polygon().size() == 0) {return;}

	m_manager->m_parent->pushRenderCommand(new PolygonPushVertexCommand(false, v->viewportToWorld(event->pos()), &m_polygonController));
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (event->button() != Qt::LeftButton) {return;}

	m_manager->m_parent->pushRenderCommand(new PolygonPushVertexCommand(true, v->viewportToWorld(event->pos()), &m_polygonController));
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::finishDefiningPolygonAndSelectPoints(bool doubleClick, bool xOr)
{
	int minCount = 4;
	if (doubleClick) {minCount = 3;}
	if (m_polygonController.polygon().count() <= minCount) {
		auto w = m_manager->m_parent->preProcessorWindow();
		QMessageBox::warning(w, GeoDataPointmap::tr("Warning"),
												 GeoDataPointmap::tr("Polygon must have three vertices at least."));
		return;
	}

	m_manager->selectPointsInsidePolygon(m_polygonController.polygon(), xOr);
	m_polygonController.clear();

	m_manager->m_parent->renderGraphicsView();
}

void GeoDataPointmap::PointsManager::SelectPointPolygonController::updateMouseCursor(VTK2DGraphicsView* v)
{
	if (m_addMode) {
		v->setCursor(m_addCursor);
	} else {
		v->setCursor(Qt::ArrowCursor);
	}
}
