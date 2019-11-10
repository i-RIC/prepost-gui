#include "polygoncontroller.h"
#include "polygonmovevertexcommand.h"

#include <misc/qundocommandhelper.h>

PolygonMoveVertexCommand::PolygonMoveVertexCommand(bool keyDown, int vertexId, const QPointF& point, PolygonController* polygon) :
	PolygonMoveVertexCommand {tr("Move Polygon Vertex"), keyDown, vertexId, point, polygon}
{}

PolygonMoveVertexCommand::PolygonMoveVertexCommand(const QString& title, bool keyDown, int vertexId, const QPointF& point, PolygonController* polygon) :
	QUndoCommand {title},
	m_keyDown {keyDown},
	m_vertexId {vertexId},
	m_newPoint {point},
	m_oldPoint {polygon->polygon().at(vertexId)},
	m_polygon {polygon}
{}

void PolygonMoveVertexCommand::redo()
{
	applyPoint(m_newPoint);
}

void PolygonMoveVertexCommand::undo()
{
	applyPoint(m_oldPoint);
}

int PolygonMoveVertexCommand::id() const
{
	return iRIC::generateCommandId("PolygonMoveVertexCommand");
}

bool PolygonMoveVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const PolygonMoveVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polygon != comm->m_polygon) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}

	m_newPoint = comm->m_newPoint;
	return true;
}

void PolygonMoveVertexCommand::applyPoint(const QPointF& point)
{
	auto pol = m_polygon->polygon();
	pol[m_vertexId] = point;
	if (m_vertexId == 0) {
		pol[pol.size() - 1] = point;
	}
	m_polygon->setPolygon(pol);
}
