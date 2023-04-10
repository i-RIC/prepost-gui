#include "geodatapointmap_pointsmanager_selectpointboundingboxcontroller.h"

#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/misc/mouseboundingbox.h>
#include <guicore/pre/base/preprocessordataitem.h>
#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <misc/mathsupport.h>

GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::SelectPointBoundingBoxController(PointsManager* manager) :
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_definingBoundingBox {false},
	m_addMode {false},
	m_manager {manager}
{}

GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::~SelectPointBoundingBoxController()
{}

void GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::activate(VTK2DGraphicsView* /*v*/)
{}

void GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::deactivate(VTK2DGraphicsView* v)
{
	m_definingBoundingBox = false;
	m_addMode = false;

	updateMouseCursor(v);

	auto box = m_manager->m_parent->dataModel()->mouseBoundingBox();
	box->disable();
}

void GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_addMode = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	updateMouseCursor(v);
}

void GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::handleKeyReleaseEvent(QKeyEvent* event, VTK2DGraphicsView* v)
{
	m_addMode = ((event->modifiers() & Qt::ShiftModifier) == Qt::ShiftModifier);
	updateMouseCursor(v);
}

void GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (! m_definingBoundingBox) {return;}

	auto box = m_manager->m_parent->dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());
	m_manager->m_parent->renderGraphicsView();
}

void GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (event->button() != Qt::LeftButton) {return;}

	m_definingBoundingBox = true;
	auto box = m_manager->m_parent->dataModel()->mouseBoundingBox();
	box->setStartPoint(event->x(), event->y());
	box->setEndPoint(event->x(), event->y());
	box->enable();
	v->GetRenderWindow()->SetDesiredUpdateRate(PreProcessorDataItem::dragUpdateRate);
}

void GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::handleMouseReleaseEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (event->button() != Qt::LeftButton) {return;}

	auto box = m_manager->m_parent->dataModel()->mouseBoundingBox();
	box->setEndPoint(event->x(), event->y());
	box->disable();
	if (iRIC::isNear(box->startPoint(), box->endPoint())) {
		QPointF worldPoint = v->viewportToWorld(event->pos());
		m_manager->selectNearestPoint(worldPoint, v->stdRadius(iRIC::nearRadius()), m_addMode);
		m_manager->m_parent->renderGraphicsView();
	} else {
		m_manager->selectPointsInsideBox(box, m_addMode);
		m_manager->m_parent->renderGraphicsView();
	}

	m_definingBoundingBox = false;
}

void GeoDataPointmap::PointsManager::SelectPointBoundingBoxController::updateMouseCursor(VTK2DGraphicsView* v)
{
	if (m_addMode) {
		v->setCursor(m_addCursor);
	} else {
		v->setCursor(Qt::ArrowCursor);
	}
}
