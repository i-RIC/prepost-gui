#include "gridcreatingconditiontriangle_addholepolygoncommand.h"
#include "../gridcreatingconditiontriangleholepolygon.h"

GridCreatingConditionTriangle::AddHolePolygonCommand::AddHolePolygonCommand(GridCreatingConditionTriangleHolePolygon* newPoly, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Add New Hole Polygon")},
	m_polygon {pol},
	m_targetPolygon {newPoly},
	m_undoed {false}
{}

GridCreatingConditionTriangle::AddHolePolygonCommand::~AddHolePolygonCommand() {
	if (m_undoed) {
		// delete m_targetPolygon;
	}
}

void GridCreatingConditionTriangle::AddHolePolygonCommand::redo()
{
	m_polygon->deselectAll();

	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meBeforeDefining;
	m_polygon->m_selectMode = GridCreatingConditionTriangle::smPolygon;
	m_targetPolygon->setActive(true);
	m_polygon->m_selectedPolygon = m_targetPolygon;
	m_polygon->m_holePolygons.append(m_targetPolygon);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->renderGraphicsView();
	m_undoed = false;
}

void GridCreatingConditionTriangle::AddHolePolygonCommand::undo()
{
	m_polygon->deselectAll();

	m_polygon->m_mouseEventMode = GridCreatingConditionTriangle::meNormal;
	m_polygon->m_holePolygons.removeOne(m_targetPolygon);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->renderGraphicsView();
	m_undoed = true;
}
