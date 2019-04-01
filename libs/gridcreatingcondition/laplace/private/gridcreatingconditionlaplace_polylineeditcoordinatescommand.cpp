#include "gridcreatingconditionlaplace_polylineeditcoordinatescommand.h"

#include <guibase/polyline/polylinecontroller.h>

GridCreatingConditionLaplace::PolyLineEditCoordinatesCommand::PolyLineEditCoordinatesCommand(const std::vector<QPointF>& coords, PolyLineController* line) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Edit Coordinates")),
	m_newCoords (coords),
	m_oldCoords (line->polyLine()),
	m_line {line}
{}

void GridCreatingConditionLaplace::PolyLineEditCoordinatesCommand::redo()
{
	m_line->setPolyLine(m_newCoords);
}

void GridCreatingConditionLaplace::PolyLineEditCoordinatesCommand::undo()
{
	m_line->setPolyLine(m_oldCoords);
}
