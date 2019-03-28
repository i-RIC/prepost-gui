#include "post2dwindowpolydatagroupdataitem_setbasicsettingcommand.h"

Post2dWindowPolyDataGroupDataItem::SetBasicSettingCommand::SetBasicSettingCommand(const PostPolyDataBasicSetting& s, Post2dWindowPolyDataGroupDataItem* item) :
	QUndoCommand {Post2dWindowPolyDataGroupDataItem::tr("Edit Polygon Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post2dWindowPolyDataGroupDataItem::SetBasicSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->updateActorSettings();
}

void Post2dWindowPolyDataGroupDataItem::SetBasicSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->updateActorSettings();
}
