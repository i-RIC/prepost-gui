#include "gridcreatingconditionlaplace_polylineremovevertexcommand.h"

#include <guibase/polyline/polylinecontroller.h>

GridCreatingConditionLaplace::PolyLineRemoveVertexCommand::PolyLineRemoveVertexCommand(int vertexId, PolyLineController* line) :
	QUndoCommand(GridCreatingConditionLaplace::tr("Remove Vertex")),
	m_vertexId {vertexId},
	m_polyLine {line}
{
	m_position = m_polyLine->polyLine().at(m_vertexId);
}

void GridCreatingConditionLaplace::PolyLineRemoveVertexCommand::redo()
{
	auto line = m_polyLine->polyLine();
	auto it = line.begin() + m_vertexId;
	line.erase(it);
	m_polyLine->setPolyLine(line);
}

void GridCreatingConditionLaplace::PolyLineRemoveVertexCommand::undo()
{
	auto line = m_polyLine->polyLine();
	auto it = line.begin() + m_vertexId;
	line.insert(it, m_position);
	m_polyLine->setPolyLine(line);
}
