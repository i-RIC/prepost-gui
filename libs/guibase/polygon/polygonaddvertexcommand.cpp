#include "polygonaddvertexcommand.h"
#include "polygoncontroller.h"

#include <misc/qundocommandhelper.h>

#include <QPolygonF>

PolygonAddVertexCommand::PolygonAddVertexCommand(bool keyDown, int edgeId, const QPointF& point, PolygonController* polygon) :
	PolygonAddVertexCommand {tr("Insert Polygon Vertex"), keyDown, edgeId, point, polygon}
{}

PolygonAddVertexCommand::PolygonAddVertexCommand(const QString& title, bool keyDown, int edgeId, const QPointF& point, PolygonController* polygon) :
	QUndoCommand {title},
	m_keyDown {keyDown},
	m_vertexId {(edgeId + 1) % polygon->polygon().size()},
	m_point {point},
	m_polygon {polygon}
{}

void PolygonAddVertexCommand::redo()
{
	auto pol = m_polygon->polygon();
	pol.insert(pol.begin() + m_vertexId, m_point);
	if (m_vertexId == 0) {
		pol.pop_back();
		pol.push_back(m_point);
	}
	m_polygon->setPolygon(pol);
}

void PolygonAddVertexCommand::undo()
{
	auto pol = m_polygon->polygon();
	pol.erase(pol.begin() + m_vertexId);
	if (m_vertexId == 0) {
		pol.pop_back();
		pol.push_back(pol.at(0));
	}
	m_polygon->setPolygon(pol);
}

int PolygonAddVertexCommand::id() const
{
	return iRIC::generateCommandId("PolygonAddVertexCommand");
}

bool PolygonAddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const auto comm = dynamic_cast<const PolygonAddVertexCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (m_polygon != comm->m_polygon ) {return false;}
	if (m_vertexId != comm->m_vertexId) {return false;}

	m_point = comm->m_point;
	return true;
}
