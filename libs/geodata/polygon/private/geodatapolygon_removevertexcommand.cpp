#include "geodatapolygon_removevertexcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::RemoveVertexCommand::RemoveVertexCommand(vtkIdType vertexId, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Remove Polygon Vertex")},
	m_newPolygon {pol->m_selectedPolygon->polygon()},
	m_oldPolygon {pol->m_selectedPolygon->polygon()},
	m_polygon {pol},
	m_targetPolygon {pol->m_selectedPolygon},
	m_oldMapped {pol->isMapped()}
{
	m_newPolygon.removeAt(vertexId);
	if (vertexId == 0) {
		m_newPolygon.removeLast();
		m_newPolygon.push_back(m_newPolygon.at(0));
	}
}

void GeoDataPolygon::RemoveVertexCommand::redo()
{
	m_polygon->setMapped(false);
	m_targetPolygon->setPolygon(m_newPolygon);
	m_polygon->renderGraphicsView();
	m_polygon->updatePolyData();
}

void GeoDataPolygon::RemoveVertexCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	m_targetPolygon->setPolygon(m_oldPolygon);
	m_polygon->renderGraphicsView();
	m_polygon->updatePolyData();
}
