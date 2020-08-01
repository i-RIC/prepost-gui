#include "geodatapolydatagroup_impl.h"
#include "geodatapolydatagroup_sortedittargetdatacommand.h"

GeoDataPolyDataGroup::SortEditTargetDataCommand::SortEditTargetDataCommand(int newIndex, GeoDataPolyDataGroup* group) :
	QUndoCommand(GeoDataPolyDataGroup::tr("Sort data")),
	m_newIndex {newIndex},
	m_oldIndex {group->impl->m_editTargetDataIndex},
	m_group {group}
{}

void GeoDataPolyDataGroup::SortEditTargetDataCommand::redo()
{
	m_group->impl->m_editTargetDataIndex = m_newIndex;
	m_group->updateAttributeBrowser();
}

void GeoDataPolyDataGroup::SortEditTargetDataCommand::undo()
{
	m_group->impl->m_editTargetDataIndex = m_oldIndex;
	m_group->updateAttributeBrowser();
}
