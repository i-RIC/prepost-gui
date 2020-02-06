#include "polylineaddvertexcommand.h"
#include "polylinecontroller.h"

#include <misc/qundocommandhelper.h>

PolyLineAddVertexCommand::PolyLineAddVertexCommand(bool keyDown, int edgeId, const QPointF& point, PolyLineController* line) :
	PolyLineAddVertexCommand {tr("Insert Line Vertex"), keyDown, edgeId, point, line}
{}

PolyLineAddVertexCommand::PolyLineAddVertexCommand(const QString& title, bool keyDown, int edgeId, const QPointF& point, PolyLineController* line) :
	QUndoCommand {title},
	m_keyDown {keyDown},
	m_vertexId {edgeId + 1},
	m_point {point},
	m_polyLine {line}
{}

void PolyLineAddVertexCommand::redo()
{
	auto line = m_polyLine->polyLine();
	if (m_keyDown) {
		// add vertex
		line.insert(line.begin() + m_vertexId, m_point);
	} else {
		// just modify the vertex position
		line[m_vertexId] = m_point;
	}
	m_polyLine->setPolyLine(line);
}

void PolyLineAddVertexCommand::undo()
{
	if (m_keyDown) {
		auto line = m_polyLine->polyLine();
		line.erase(line.begin() + m_vertexId);
		m_polyLine->setPolyLine(line);
	} else {
		// this never happens.
	}
}

int PolyLineAddVertexCommand::id() const
{
	return iRIC::generateCommandId("PolyLineAddVertex");
}

bool PolyLineAddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const PolyLineAddVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polyLine != comm->m_polyLine) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}

	m_point = comm->m_point;
	return true;
}
