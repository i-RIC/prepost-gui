#include "gridcreatingconditioncenterandwidth_movevertexcommand.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

#include <vtkPolyData.h>

GridCreatingConditionCenterAndWidth::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCenterAndWidth* cond) :
	PolyLineMoveVertexCommand {GridCreatingConditionCenterAndWidth::tr("Move Center Line Vertex"), keyDown, vertexId,
														(cond->m_polyLineController.polyLine().at(vertexId) + cond->graphicsView()->viewportToWorld(to) - cond->graphicsView()->viewportToWorld(from)),
														&(cond->m_polyLineController)},
	m_condition {cond}
{}

void GridCreatingConditionCenterAndWidth::MoveVertexCommand::redo()
{
	PolyLineMoveVertexCommand::redo();
	if (m_condition->m_isGridCreated) {
		m_condition->createSpline(m_condition->m_polyLineController.polyData()->GetPoints(), m_condition->m_iMax - 1);
		emit m_condition->tmpGridCreated(m_condition->createGrid());
	}
}

void GridCreatingConditionCenterAndWidth::MoveVertexCommand::undo()
{
	PolyLineMoveVertexCommand::undo();
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
	if (comm->m_condition != m_condition) {return false;}
	return PolyLineMoveVertexCommand::mergeWith(other);
}
