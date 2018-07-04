#include "gridcreatingconditioncenterandwidth_editcoordinatescommand.h"

GridCreatingConditionCenterAndWidth::EditCoordinatesCommand::EditCoordinatesCommand(const std::vector<QPointF>& coords, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Edit Center Line Coordinates")},
	m_newCoords (coords),
	m_oldCoords (cond->polyLine()),
	m_condition {cond}
{}

void GridCreatingConditionCenterAndWidth::EditCoordinatesCommand::redo()
{
	m_condition->setPolyLine(m_newCoords);
}

void GridCreatingConditionCenterAndWidth::EditCoordinatesCommand::undo()
{
	m_condition->setPolyLine(m_oldCoords);
}
