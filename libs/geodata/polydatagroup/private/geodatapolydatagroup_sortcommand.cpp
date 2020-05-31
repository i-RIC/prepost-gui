#include "geodatapolydatagroup_impl.h"
#include "geodatapolydatagroup_sortcommand.h"

GeoDataPolyDataGroup::SortCommand::SortCommand(const std::vector<GeoDataPolyDataGroupPolyData*> newData, GeoDataPolyDataGroup* group) :
	QUndoCommand(GeoDataPolyDataGroup::tr("Sort data")),
	m_newData (newData),
	m_oldData (group->data()),
	m_group {group}
{}

void GeoDataPolyDataGroup::SortCommand::redo()
{
	m_group->impl->m_data = m_newData;
	m_group->updateAttributeBrowser();
}

void GeoDataPolyDataGroup::SortCommand::undo()
{
	m_group->impl->m_data = m_oldData;
	m_group->updateAttributeBrowser();
}
