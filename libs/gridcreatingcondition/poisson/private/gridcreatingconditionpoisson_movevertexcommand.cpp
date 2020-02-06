#include "gridcreatingconditionpoisson_impl.h"
#include "gridcreatingconditionpoisson_movevertexcommand.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionPoisson::MoveVertexCommand::MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, int vertexId, GridCreatingConditionPoisson* cond) :
	QUndoCommand {GridCreatingConditionPoisson::tr("Move Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_polyLine {cond->impl->m_activePoints}
{
	double x = from.x();
	double y = from.y();
	cond->graphicsView()->viewportToWorld(x, y);
	QPointF fromVec(x, y);
	x = to.x();
	y = to.y();
	cond->graphicsView()->viewportToWorld(x, y);
	QPointF toVec(x, y);
	m_offset = toVec - fromVec;
}

void GridCreatingConditionPoisson::MoveVertexCommand::redo()
{
	auto line = m_polyLine->polyLine();
	line[m_vertexId] += m_offset;
	m_polyLine->setPolyLine(line);
}

void GridCreatingConditionPoisson::MoveVertexCommand::undo()
{
	auto line = m_polyLine->polyLine();
	line[m_vertexId] -= m_offset;
	m_polyLine->setPolyLine(line);
}

int GridCreatingConditionPoisson::MoveVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionPoissonMoveVertex");
}

bool GridCreatingConditionPoisson::MoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MoveVertexCommand* comm = dynamic_cast<const MoveVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polyLine != m_polyLine) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}

	m_offset += comm->m_offset;
	return true;
}
