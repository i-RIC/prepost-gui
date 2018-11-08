#include "gridcreatingconditioncompoundchannel_movepolylinevertexcommand.h"
#include "../gridcreatingconditioncompoundchannelabstractline.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GridCreatingConditionCompoundChannel::MovePolyLineVertexCommand::MovePolyLineVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Move Polygonal Line Vertex")},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_targetLine {cond->m_selectedLine},
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

void GridCreatingConditionCompoundChannel::MovePolyLineVertexCommand::redo()
{
	applyOffset(m_offset);
}

void GridCreatingConditionCompoundChannel::MovePolyLineVertexCommand::undo()
{
	applyOffset(- m_offset);
}

int GridCreatingConditionCompoundChannel::MovePolyLineVertexCommand::id() const
{
	return iRIC::generateCommandId("GridCreatingConditionCompoundChannelPolyLineMoveVertex");
}

bool GridCreatingConditionCompoundChannel::MovePolyLineVertexCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolyLineVertexCommand* comm = dynamic_cast<const MovePolyLineVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_targetLine != m_targetLine) {return false;}
	if (comm->m_condition != m_condition) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}
	m_offset += comm->m_offset;
	return true;
}

void GridCreatingConditionCompoundChannel::MovePolyLineVertexCommand::applyOffset(const QPointF& offset)
{
	QPointF p = m_targetLine->vertex(m_vertexId);
	p += offset;
	m_targetLine->setVertex(m_vertexId, p);
}
