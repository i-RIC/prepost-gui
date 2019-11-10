#include "gridcreatingconditioncompoundchannel_movepolygonvertexcommand.h"
#include "../gridcreatingconditioncompoundchannelabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GridCreatingConditionCompoundChannel::MovePolygonVertexCommand::MovePolygonVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Move Polygon Vertex")},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_targetPolygon {cond->m_selectedPolygon},
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

void GridCreatingConditionCompoundChannel::MovePolygonVertexCommand::redo()
{
	applyOffset(m_offset);
}

void GridCreatingConditionCompoundChannel::MovePolygonVertexCommand::undo()
{
	applyOffset(- m_offset);
}

int GridCreatingConditionCompoundChannel::MovePolygonVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCompoundChannelPolygonMoveVertex");
}

bool GridCreatingConditionCompoundChannel::MovePolygonVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolygonVertexCommand* comm = dynamic_cast<const MovePolygonVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}
	if (comm->m_condition != m_condition) {return false;}
	m_offset += comm->m_offset;
	return true;
}

void GridCreatingConditionCompoundChannel::MovePolygonVertexCommand::applyOffset(const QPointF& offset)
{
	QPointF p = m_targetPolygon->vertex(m_vertexId);
	p += offset;
	m_targetPolygon->setVertex(m_vertexId, p);
}
