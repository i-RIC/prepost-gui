#include "gridcreatingconditionlaplace_polylinemovevertexcommand.h"

#include <guibase/polyline/polylinecontroller.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionLaplace::PolyLineMoveVertexCommand::PolyLineMoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, PolyLineController* line, int vertexId, GridCreatingConditionLaplace* cond) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Move Line Vertex")),
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_polyline {line}
{
	m_oldPosition = m_polyline->polyLine().at(m_vertexId);

	double x = from.x();
	double y = from.y();
	cond->graphicsView()->viewportToWorld(x, y);
	QPointF fromVec(x, y);
	x = to.x();
	y = to.y();
	cond->graphicsView()->viewportToWorld(x, y);
	QPointF toVec(x, y);

	m_newPosition = m_oldPosition + toVec - fromVec;
}

void GridCreatingConditionLaplace::PolyLineMoveVertexCommand::redo()
{
	auto line = m_polyline->polyLine();
	line[m_vertexId] = m_newPosition;
	m_polyline->setPolyLine(line);
}

void GridCreatingConditionLaplace::PolyLineMoveVertexCommand::undo()
{
	auto line = m_polyline->polyLine();
	line[m_vertexId] = m_oldPosition;
	m_polyline->setPolyLine(line);
}

int GridCreatingConditionLaplace::PolyLineMoveVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionLaplacePolyLineMoveVertex");
}

bool GridCreatingConditionLaplace::PolyLineMoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto* comm = dynamic_cast<const PolyLineMoveVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polyline != m_polyline) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}

	m_newPosition = comm->m_newPosition;
	return true;
}
