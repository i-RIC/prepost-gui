#include "gridcreatingconditioncenterandwidth_movevertexcommand.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionCenterAndWidth::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Move Center Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_condition {cond}
{
	double dx = from.x();
	double dy = from.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QVector2D fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QVector2D toVec(dx, dy);
	m_offset = toVec - fromVec;
}

void GridCreatingConditionCenterAndWidth::MoveVertexCommand::redo()
{
	vtkPolyLine* line = m_condition->m_vtkPolyLine;
	vtkPoints* points = line->GetPoints();
	double p[3];
	points->GetPoint(m_vertexId, p);
	p[0] += m_offset.x();
	p[1] += m_offset.y();
	points->SetPoint(m_vertexId, p);

	points->Modified();
	line->Modified();
	m_condition->updateShapeData();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::MoveVertexCommand::undo()
{
	vtkPolyLine* line = m_condition->m_vtkPolyLine;
	vtkPoints* points = line->GetPoints();
	double p[3];
	points->GetPoint(m_vertexId, p);
	p[0] -= m_offset.x();
	p[1] -= m_offset.y();
	points->SetPoint(m_vertexId, p);

	points->Modified();
	line->Modified();
	m_condition->updateShapeData();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_vtkPolyLine->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

int GridCreatingConditionCenterAndWidth::MoveVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCenterAndWidthMoveVertex");
}

bool GridCreatingConditionCenterAndWidth::MoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MoveVertexCommand* comm = dynamic_cast<const MoveVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_condition != m_condition) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}

	m_offset += comm->m_offset;
	return true;
}
