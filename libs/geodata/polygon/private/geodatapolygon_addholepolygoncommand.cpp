#include "geodatapolygon_addholepolygoncommand.h"

#include "../geodatapolygonholepolygon.h"

GeoDataPolygon::AddHolePolygonCommand::AddHolePolygonCommand(GeoDataPolygonHolePolygon* newPoly, GeoDataPolygon* pol) :
	QUndoCommand {GeoDataPolygon::tr("Add New Hole Polygon")}
{
	m_polygon = pol;
	m_targetPolygon = newPoly;
	m_undoed = false;
	m_oldMapped = pol->isMapped();
}

GeoDataPolygon::AddHolePolygonCommand::~AddHolePolygonCommand()
{
	if (m_undoed) {
//	delete m_targetPolygon;
	}
}

void GeoDataPolygon::AddHolePolygonCommand::redo()
{
	m_polygon->setMapped(false);
	m_polygon->deselectAll();

	m_polygon->m_mouseEventMode = GeoDataPolygon::meBeforeDefining;
	m_polygon->m_selectMode = GeoDataPolygon::smPolygon;
	m_targetPolygon->setSelected(true);
	m_polygon->m_selectedPolygon = m_targetPolygon;
	m_polygon->m_holePolygons.append(m_targetPolygon);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_undoed = false;
}

void GeoDataPolygon::AddHolePolygonCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	m_polygon->deselectAll();
	m_polygon->m_mouseEventMode = GeoDataPolygon::meNormal;
	m_polygon->m_holePolygons.removeOne(m_targetPolygon);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_undoed = true;
}
