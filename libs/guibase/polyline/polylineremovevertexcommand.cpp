#include "polylinecontroller.h"
#include "polylineremovevertexcommand.h"

PolyLineRemoveVertexCommand::PolyLineRemoveVertexCommand(int vertexId, PolyLineController* line) :
	PolyLineRemoveVertexCommand {tr("Remove Line Vertex"), vertexId, line}
{}

PolyLineRemoveVertexCommand::PolyLineRemoveVertexCommand(const QString &title, int vertexId, PolyLineController* line) :
	QUndoCommand {title},
	m_vertexId {vertexId},
	m_point {line->polyLine().at(vertexId)},
	m_polyLine {line}
{}

void PolyLineRemoveVertexCommand::redo()
{
	auto line = m_polyLine->polyLine();
	auto it = line.begin() + m_vertexId;
	line.erase(it);
	m_polyLine->setPolyLine(line);
}

void PolyLineRemoveVertexCommand::undo()
{
	auto line = m_polyLine->polyLine();
	auto it = line.begin() + m_vertexId;
	line.insert(it, m_point);
	m_polyLine->setPolyLine(line);
}
