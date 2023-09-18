#include "geodatapointmap_pointsmanager_interpolatepointcontroller_addpointcommand.h"

#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>

GeoDataPointmap::PointsManager::InterpolatePointController::AddPointCommand::AddPointCommand(const QPointF& point, double value, bool newFlag, InterpolatePointController* controller) :
	m_point {point},
	m_value {value},
	m_newFlag {newFlag},
	m_controller {controller}
{}

void GeoDataPointmap::PointsManager::InterpolatePointController::AddPointCommand::redo()
{
	auto line = m_controller->m_pointsController.polyLine();
	line.push_back(m_point);
	m_controller->m_pointsController.setPolyLine(line);

	m_controller->m_values.push_back(m_value);
	m_controller->m_newFlags.push_back(m_newFlag);
	m_controller->m_mouseEventMode = MouseEventMode::AddNewPoint;

	auto v = m_controller->m_manager->m_parent->dataModel()->graphicsView();
	m_controller->updateMouseCursor(v);
}

void GeoDataPointmap::PointsManager::InterpolatePointController::AddPointCommand::undo()
{
	auto line = m_controller->m_pointsController.polyLine();
	line.pop_back();
	m_controller->m_pointsController.setPolyLine(line);

	m_controller->m_values.pop_back();
	m_controller->m_newFlags.pop_back();

	if (m_controller->m_pointsController.polyLine().size() == 0) {
		m_controller->m_mouseEventMode = MouseEventMode::SelectFirstPointNotPossible;
	} else {
		m_controller->m_mouseEventMode = MouseEventMode::AddNewPoint;
	}

	auto v = m_controller->m_manager->m_parent->dataModel()->graphicsView();
	m_controller->updateMouseCursor(v);
}
