#include "gridcreatingconditionpoisson_impl.h"
#include "gridcreatingconditionpoisson_removevertexcommand.h"

GridCreatingConditionPoisson::RemoveVertexCommand::RemoveVertexCommand(int vertexId, GridCreatingConditionPoisson* cond) :
	QUndoCommand {GridCreatingConditionPoisson::tr("Remove Vertex")},
	m_vertexId {vertexId},
	m_polyLine {cond->impl->m_activeLine},
	m_condition {cond}
{
	m_vertexPosition = m_polyLine->polyLine().at(m_vertexId);
}

void GridCreatingConditionPoisson::RemoveVertexCommand::redo()
{
	auto line = m_polyLine->polyLine();
	auto it = line.begin() + m_vertexId;
	line.erase(it);
	m_polyLine->setPolyLine(line);
}

void GridCreatingConditionPoisson::RemoveVertexCommand::undo()
{
	auto line = m_polyLine->polyLine();
	auto it = line.begin() + m_vertexId;
	line.insert(it, m_vertexPosition);
	m_polyLine->setPolyLine(line);
}
