#include "gridcreatingconditiontriangle_movepolygonvertexcommand.h"
#include "../gridcreatingconditiontriangleabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionTriangle::MovePolygonVertexCommand::MovePolygonVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Move Polygon Vertex")},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_polygon {pol},
	m_targetPolygon {pol->m_selectedPolygon}
{
	double dx = from.x();
	double dy = from.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QVector2D fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QVector2D toVec(dx, dy);
	m_offset = toVec - fromVec;
}

void GridCreatingConditionTriangle::MovePolygonVertexCommand::redo()
{
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	vtkPoints* points = pol->GetPoints();
	double p[3];
	points->GetPoint(m_vertexId, p);
	p[0] += m_offset.x();
	p[1] += m_offset.y();
	points->SetPoint(m_vertexId, p);

	points->Modified();
	pol->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->renderGraphicsView();
}

void GridCreatingConditionTriangle::MovePolygonVertexCommand::undo()
{
	vtkPolygon* pol = m_targetPolygon->getVtkPolygon();
	vtkPoints* points = pol->GetPoints();
	double p[3];
	points->GetPoint(m_vertexId, p);
	p[0] -= m_offset.x();
	p[1] -= m_offset.y();
	points->SetPoint(m_vertexId, p);

	points->Modified();
	pol->Modified();
	m_targetPolygon->updateShapeData();
	m_polygon->renderGraphicsView();
}

int GridCreatingConditionTriangle::MovePolygonVertexCommand::id() const
{
	return iRIC::generateCommandId("MovePolygonVertex");
}

bool GridCreatingConditionTriangle::MovePolygonVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolygonVertexCommand* comm = dynamic_cast<const MovePolygonVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polygon != m_polygon) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}
	m_offset += comm->m_offset;
	return true;
}
