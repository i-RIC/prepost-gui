#include "gridcreatingconditiontriangle_finishpolylinedefiningcommand.h"
#include "../gridcreatingconditiontriangleabstractline.h"

GridCreatingConditionTriangle::FinishPolyLineDefiningCommand::FinishPolyLineDefiningCommand(GridCreatingConditionTriangle* polygon) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Finish Defining Break Line")},
	m_polygon {polygon},
	m_targetLine {polygon->m_selectedLine}
{}

void GridCreatingConditionTriangle::FinishPolyLineDefiningCommand::redo()
{
	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
	m_targetLine->finishDefinition();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->updateActionStatus();
}

void GridCreatingConditionTriangle::FinishPolyLineDefiningCommand::undo()
{
	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meDefining;
	m_polygon->m_selectMode = GridCreatingConditionTriangle::smLine;
	m_polygon->m_selectedLine = m_targetLine;
	m_polygon->m_selectedLine->setActive(true);
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->updateActionStatus();
}
