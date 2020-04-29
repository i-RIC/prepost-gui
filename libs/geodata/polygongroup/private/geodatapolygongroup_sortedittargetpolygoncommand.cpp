#include "geodatapolygongroup_impl.h"
#include "geodatapolygongroup_sortedittargetpolygoncommand.h"

GeoDataPolygonGroup::SortEditTargetPolygonCommand::SortEditTargetPolygonCommand(int newIndex, GeoDataPolygonGroup* group) :
	QUndoCommand(GeoDataPolygonGroup::tr("Sort polygons")),
	m_newIndex {newIndex},
	m_oldIndex {group->impl->m_editTargetPolygonIndex},
	m_group {group}
{}

void GeoDataPolygonGroup::SortEditTargetPolygonCommand::redo()
{
	m_group->impl->m_editTargetPolygonIndex = m_newIndex;
	m_group->updateAttributeBrowser();
}

void GeoDataPolygonGroup::SortEditTargetPolygonCommand::undo()
{
	m_group->impl->m_editTargetPolygonIndex = m_oldIndex;
	m_group->updateAttributeBrowser();
}
