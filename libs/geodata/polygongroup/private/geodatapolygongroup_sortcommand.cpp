#include "geodatapolygongroup_impl.h"
#include "geodatapolygongroup_sortcommand.h"

GeoDataPolygonGroup::SortCommand::SortCommand(const std::vector<GeoDataPolygonGroupPolygon*> newPols, GeoDataPolygonGroup* group) :
	QUndoCommand(GeoDataPolygonGroup::tr("Sort polygons")),
	m_newPolygons (newPols),
	m_oldPolygons (group->allPolygons()),
	m_group {group}
{}

void GeoDataPolygonGroup::SortCommand::redo()
{
	m_group->impl->m_polygons = m_newPolygons;
	m_group->updateAttributeBrowser();
}

void GeoDataPolygonGroup::SortCommand::undo()
{
	m_group->impl->m_polygons = m_oldPolygons;
	m_group->updateAttributeBrowser();
}
