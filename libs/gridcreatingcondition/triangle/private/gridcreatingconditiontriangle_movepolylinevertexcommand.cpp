#include "gridcreatingconditiontriangle_movepolylinevertexcommand.h"
#include "../gridcreatingconditiontriangleabstractline.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionTriangle::MovePolyLineVertexCommand::MovePolyLineVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionTriangle* pol) :
	QUndoCommand {GridCreatingConditionTriangle::tr("Move Break Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_polygon {pol},
	m_targetLine {pol->m_selectedLine}
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

void GridCreatingConditionTriangle::MovePolyLineVertexCommand::redo()
{
	vtkPolyLine* pol = m_targetLine->getVtkLine();
	vtkPoints* points = pol->GetPoints();
	double p[3];
	points->GetPoint(m_vertexId, p);
	p[0] += m_offset.x();
	p[1] += m_offset.y();
	points->SetPoint(m_vertexId, p);

	points->Modified();
	pol->Modified();
	m_targetLine->updateShapeData();
	m_polygon->renderGraphicsView();
}

void GridCreatingConditionTriangle::MovePolyLineVertexCommand::undo()
{
	vtkPolyLine* pol = m_targetLine->getVtkLine();
	vtkPoints* points = pol->GetPoints();
	double p[3];
	points->GetPoint(m_vertexId, p);
	p[0] -= m_offset.x();
	p[1] -= m_offset.y();
	points->SetPoint(m_vertexId, p);

	points->Modified();
	pol->Modified();
	m_targetLine->updateShapeData();
	m_polygon->renderGraphicsView();
}

int GridCreatingConditionTriangle::MovePolyLineVertexCommand::id() const
{
	return iRIC::generateCommandId("PolyLineMoveVertex");
}

bool GridCreatingConditionTriangle::MovePolyLineVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolyLineVertexCommand* comm = dynamic_cast<const MovePolyLineVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polygon != m_polygon) {return false;}
	if (comm->m_targetLine != m_targetLine) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}
	m_offset += comm->m_offset;
	return true;
}
