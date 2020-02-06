#include "polylinecontroller.h"
#include "polylinemovevertexcommand.h"

#include <misc/qundocommandhelper.h>

PolyLineMoveVertexCommand::PolyLineMoveVertexCommand(bool keyDown, int vertexId, const QPointF& point, PolyLineController* line) :
	PolyLineMoveVertexCommand {tr("Move Line Vertex"), keyDown, vertexId, point, line}
{}

PolyLineMoveVertexCommand::PolyLineMoveVertexCommand(const QString& title, bool keyDown, int vertexId, const QPointF& point, PolyLineController* line) :
	QUndoCommand {title},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_newPoint {point},
	m_oldPoint {line->polyLine().at(vertexId)},
	m_polyLine {line}
{}

void PolyLineMoveVertexCommand::redo()
{
	applyPoint(m_newPoint);
}

void PolyLineMoveVertexCommand::undo()
{
	applyPoint(m_oldPoint);
}

int PolyLineMoveVertexCommand::id() const
{
	return iRIC::generateCommandId("PolyLineMoveVertexCommand");
}

bool PolyLineMoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const PolyLineMoveVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polyLine != m_polyLine) {return false;}
	if (comm->m_vertexId != m_vertexId) {return false;}

	m_newPoint = comm->m_newPoint;
	return true;
}

void PolyLineMoveVertexCommand::applyPoint(const QPointF& point)
{
	auto line = m_polyLine->polyLine();
	line[m_vertexId] = point;
	m_polyLine->setPolyLine(line);
}
