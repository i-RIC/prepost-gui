#include "gridcreatingconditiontriangle_finishpolygondefiningcommand.h"
#include "../gridcreatingconditiontriangleabstractpolygon.h"

GridCreatingConditionTriangle::FinishPolygonDefiningCommand::FinishPolygonDefiningCommand(GridCreatingConditionTriangle* polygon) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Finish Defining Polygon")},
	m_polygon {polygon},
	m_targetPolygon {polygon->m_selectedPolygon}
{}

void GridCreatingConditionTriangle::FinishPolygonDefiningCommand::redo()
{
	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
	m_targetPolygon->finishDefinition();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->updateActionStatus();
}

void GridCreatingConditionTriangle::FinishPolygonDefiningCommand::undo()
{
	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meDefining;
	m_polygon->m_selectedPolygon = m_targetPolygon;
	m_polygon->m_selectedPolygon->setActive(true);
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->updateActionStatus();
}
