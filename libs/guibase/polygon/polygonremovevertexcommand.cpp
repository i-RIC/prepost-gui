#include "polygoncontroller.h"
#include "polygonremovevertexcommand.h"

#include <QPolygonF>

PolygonRemoveVertexCommand::PolygonRemoveVertexCommand(int vertexId, PolygonController* polygon) :
	PolygonRemoveVertexCommand {tr("Remove Polygon Vertex"), vertexId, m_polygon}
{}

PolygonRemoveVertexCommand::PolygonRemoveVertexCommand(const QString& title, int vertexId, PolygonController* polygon) :
	QUndoCommand {title},
	m_vertexId {vertexId},
	m_point {polygon->polygon().at(vertexId)},
	m_polygon {polygon}
{}

void PolygonRemoveVertexCommand::redo()
{
	auto pol = m_polygon->polygon();
	pol.erase(pol.begin() + m_vertexId);
	if (m_vertexId == 0) {
		pol.pop_back();
		pol.push_back(pol.at(0));
	}
	m_polygon->setPolygon(pol);
}

void PolygonRemoveVertexCommand::undo()
{
	auto pol = m_polygon->polygon();
	pol.insert(pol.begin() + m_vertexId, m_point);
	if (m_vertexId == 0) {
		pol.pop_back();
		pol.push_back(m_point);
	}
	m_polygon->setPolygon(pol);
}
