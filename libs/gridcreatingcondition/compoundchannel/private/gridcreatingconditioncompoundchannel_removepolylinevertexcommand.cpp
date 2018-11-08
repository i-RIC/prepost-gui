#include "gridcreatingconditioncompoundchannel_removepolylinevertexcommand.h"
#include "../gridcreatingconditioncompoundchannelabstractline.h"

GridCreatingConditionCompoundChannel::RemovePolyLineVertexCommand::RemovePolyLineVertexCommand(vtkIdType vertexId, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Remove Polygonal Line Vertex")},
	m_vertexId {vertexId},
	m_vertexPosition {cond->m_selectedLine->vertex(m_vertexId)},
	m_targetLine {cond->m_selectedLine},
	m_condition {cond}
{}

void GridCreatingConditionCompoundChannel::RemovePolyLineVertexCommand::redo()
{
	m_targetLine->removeVertex(m_vertexId);
	m_condition->m_mouseEventMode = GridCreatingConditionCompoundChannel::meNormal;
}

void GridCreatingConditionCompoundChannel::RemovePolyLineVertexCommand::undo()
{
	m_targetLine->insertVertex(m_vertexId, m_vertexPosition);
}
