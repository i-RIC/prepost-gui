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

	m_polygon->setMouseEventMode(meBeforeDefining);
	m_polygon->setSelectMode(smPolygon);
	m_targetPolygon->setSelected(true);
	m_polygon->setSelectedPolygon(m_targetPolygon);
	m_polygon->holePolygons().push_back(m_targetPolygon);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_undoed = false;
}

void GeoDataPolygon::AddHolePolygonCommand::undo()
{
	m_polygon->setMapped(m_oldMapped);
	m_polygon->deselectAll();
	m_polygon->setMouseEventMode(meNormal);
	m_polygon->holePolygons().removeOne(m_targetPolygon);
	m_polygon->updateActionStatus();
	m_polygon->updateMouseCursor(m_polygon->graphicsView());
	m_undoed = true;
}
