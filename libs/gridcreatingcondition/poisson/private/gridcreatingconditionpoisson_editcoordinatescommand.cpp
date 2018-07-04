#include "gridcreatingconditionpoisson_editcoordinatescommand.h"

#include <guibase/polyline/polylinecontroller.h>

#include <QPointF>

GridCreatingConditionPoisson::EditCoordinatesCommand::EditCoordinatesCommand(const QString& lineName, const std::vector<QPointF>& coords, PolyLineController* line) :
	QUndoCommand {GridCreatingConditionPoisson::tr("Edit %1 Coordinates").arg(lineName)},
	m_newCoords (coords),
	m_oldCoords (line->polyLine()),
	m_line {line}
{}

void GridCreatingConditionPoisson::EditCoordinatesCommand::redo()
{
	m_line->setPolyLine(m_newCoords);
}

void GridCreatingConditionPoisson::EditCoordinatesCommand::undo()
{
	m_line->setPolyLine(m_oldCoords);
}
