#include "gridcreatingconditiontriangle_adddivisionlinecommand.h"
#include "../gridcreatingconditiontriangledivisionline.h"

GridCreatingConditionTriangle::AddDivisionLineCommand::AddDivisionLineCommand(GridCreatingConditionTriangleDivisionLine* newLine, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Add New Break Line")},
	m_polygon {pol},
	m_targetLine {newLine},
	m_undoed {false}
{}

GridCreatingConditionTriangle::AddDivisionLineCommand::~AddDivisionLineCommand()
{
	if (m_undoed) {
		// delete m_targetLine;
	}
}

void GridCreatingConditionTriangle::AddDivisionLineCommand::redo()
{
	m_polygon->deselectAll();

	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meBeforeDefining;
	m_polygon->m_selectMode = GridCreatingConditionTriangle::smLine;
	m_targetLine->setActive(true);
	m_polygon->m_selectedLine = m_targetLine;
	m_polygon->m_divisionLines.append(m_targetLine);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->renderGraphicsView();
	m_undoed = false;
}

void GridCreatingConditionTriangle::AddDivisionLineCommand::undo()
{
	m_polygon->deselectAll();

	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
	m_polygon->m_divisionLines.removeOne(m_targetLine);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->renderGraphicsView();
	m_undoed = true;
}
