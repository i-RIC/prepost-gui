#include "geodatapolygon_finishpolygondefinitioncommand.h"
#include "../geodatapolygonabstractpolygon.h"

GeoDataPolygon::FinishPolygonDefiningCommand::FinishPolygonDefiningCommand(GeoDataPolygon* polygon) :
	QUndoCommand {GeoDataPolygon::tr("Finish Defining Polygon")}
{
	m_polygon = polygon;
	m_targetPolygon = m_polygon->m_selectedPolygon;
}

void GeoDataPolygon::FinishPolygonDefiningCommand::redo()
{
	m_polygon->m_mouseEventMode = GeoDataPolygon::meNormal;
	m_targetPolygon->finishDefinition();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->updateActionStatus();
}

void GeoDataPolygon::FinishPolygonDefiningCommand::undo()
{
	m_polygon->m_mouseEventMode = GeoDataPolygon::meDefining;
	m_polygon->m_selectedPolygon = m_targetPolygon;
	m_polygon->m_selectedPolygon->setSelected(true);
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_polygon->updateActionStatus();
}
