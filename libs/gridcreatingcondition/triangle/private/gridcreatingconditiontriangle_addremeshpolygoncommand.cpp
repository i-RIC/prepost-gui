#include "gridcreatingconditiontriangle_addremeshpolygoncommand.h"
#include "../gridcreatingconditiontriangleremeshpolygon.h"

GridCreatingConditionTriangle::AddRemeshPolygonCommand::AddRemeshPolygonCommand(GridCreatingConditionTriangleRemeshPolygon* newPoly, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Add New Remesh Polygon")},
	m_polygon {pol},
	m_targetPolygon {newPoly},
	m_undoed {false}
{}

GridCreatingConditionTriangle::AddRemeshPolygonCommand::~AddRemeshPolygonCommand()
{
	if (m_undoed) {
		// delete m_targetPolygon;
	}
}

void GridCreatingConditionTriangle::AddRemeshPolygonCommand::redo()
{
	m_polygon->deselectAll();

	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meBeforeDefining;
	m_polygon->m_selectMode = GridCreatingConditionTriangle::smPolygon;
	m_targetPolygon->setActive(true);
	m_polygon->m_selectedPolygon = m_targetPolygon;
	m_polygon->m_remeshPolygons.append(m_targetPolygon);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->renderGraphicsView();
	m_undoed = false;
}

void GridCreatingConditionTriangle::AddRemeshPolygonCommand::undo()
{
	m_polygon->deselectAll();

	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
	m_polygon->m_remeshPolygons.removeOne(m_targetPolygon);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->renderGraphicsView();
	m_undoed = true;
}
