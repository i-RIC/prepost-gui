#include "polylineeditcoordinatescommand.h"
#include "polylinecontroller.h"

PolyLineEditCoordinatesCommand::PolyLineEditCoordinatesCommand(const std::vector<QPointF>& coords, PolyLineController* line) :
	PolyLineEditCoordinatesCommand {tr("Edit Line Coordinates"), coords, line}
{}

PolyLineEditCoordinatesCommand::PolyLineEditCoordinatesCommand(const QString& title, const std::vector<QPointF>& coords, PolyLineController* line) :
	QUndoCommand {title},
	m_newCoords (coords),
	m_oldCoords (line->polyLine()),
	m_line {line}
{}


void PolyLineEditCoordinatesCommand::redo()
{
	m_line->setPolyLine(m_newCoords);
}

void PolyLineEditCoordinatesCommand::undo()
{
	m_line->setPolyLine(m_oldCoords);
}
