#include "geodatapointmap_pointsmanager_addpointcontroller_setexistingpointcommand.h"

#include <guicore/pre/base/preprocessordatamodelinterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPointmap::PointsManager::AddPointController::SetExistingPointCommand::SetExistingPointCommand(vtkIdType pointId, double value, AddPointController* controller) :
	m_pointId {pointId},
	m_value {value},
	m_controller {controller}
{}

void GeoDataPointmap::PointsManager::AddPointController::SetExistingPointCommand::redo()
{
	m_controller->m_existingPointId = m_pointId;
	m_controller->m_existingPointValue = m_value;
	m_controller->m_mouseEventMode = MouseEventMode::AddPoint;

	auto v = m_controller->m_manager->m_parent->dataModel()->graphicsView();
	m_controller->updateMouseCursor(v);

	double p[3];
	m_controller->m_manager->m_points->GetPoint(m_pointId, p);

	QPointF point(p[0], p[1]);
	std::vector<QPointF> points;
	points.push_back(point);

	m_controller->m_pointsController.setPolyLine(points);
}

void GeoDataPointmap::PointsManager::AddPointController::SetExistingPointCommand::undo()
{
	m_controller->m_mouseEventMode = MouseEventMode::SelectExistingPointNotPossible;
	auto v = m_controller->m_manager->m_parent->dataModel()->graphicsView();
	m_controller->updateMouseCursor(v);

	m_controller->m_pointsController.clear();
}
