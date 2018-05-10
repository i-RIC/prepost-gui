#include "gridcreatingconditioncompoundchannel_removepolygonvertexcommand.h"
#include "../gridcreatingconditioncompoundchannelabstractpolygon.h"

GridCreatingConditionCompoundChannel::RemovePolygonVertexCommand::RemovePolygonVertexCommand(vtkIdType vertexId, GridCreatingConditionCompoundChannel* cond) :
	QUndoCommand {GridCreatingConditionCompoundChannel::tr("Remove Polygon Vertex")},
	m_vertexId {vertexId},
	m_vertexPosition {cond->m_selectedPolygon->vertex(m_vertexId)},
	m_targetPolygon {cond->m_selectedPolygon},
	m_condition {cond}
{}

void GridCreatingConditionCompoundChannel::RemovePolygonVertexCommand::redo()
{
	m_targetPolygon->removeVertex(m_vertexId);
	m_condition->m_mouseEventMode = GridCreatingConditionCompoundChannel::meNormal;
}

void GridCreatingConditionCompoundChannel::RemovePolygonVertexCommand::undo() {
	m_targetPolygon->insertVertex(m_vertexId, m_vertexPosition);
}
