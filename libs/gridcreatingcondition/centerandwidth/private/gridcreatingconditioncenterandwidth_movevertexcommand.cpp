#include "gridcreatingconditioncenterandwidth_movevertexcommand.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond) :
	QUndoCommand {GridCreatingConditionCenterAndWidth::tr("Move Center Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_condition {cond}
{
	double dx = from.x();
	double dy = from.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QPointF fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	cond->graphicsView()->viewportToWorld(dx, dy);
	QPointF toVec(dx, dy);
	m_offset = toVec - fromVec;
}

void GridCreatingConditionCenterAndWidth::MoveVertexCommand::redo()
{
	auto line = m_condition->polyLine();
	line[m_vertexId] += m_offset;
	m_condition->setPolyLine(line);
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::MoveVertexCommand::undo()
{
	auto line = m_condition->polyLine();
	line[m_vertexId] -= m_offset;
	m_condition->setPolyLine(line);
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
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
