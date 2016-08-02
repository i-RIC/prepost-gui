#include "post2dwindowgraphgroupdataitem_setsettingcommand.h"
#include "post2dwindowgraphgroupdataitem_impl.h"

Post2dWindowGraphGroupDataItem::SetSettingCommand::SetSettingCommand(const Post2dWindowGraphSetting& newSetting, Post2dWindowGraphGroupDataItem* item) :
	QUndoCommand(Post2dWindowGraphGroupDataItem::tr("Update Graph Setting")),
	m_newSetting {newSetting},
	m_oldSetting {item->impl->m_setting},
	m_item {item}
{}

void Post2dWindowGraphGroupDataItem::SetSettingCommand::redo()
{
	m_item->impl->m_setting = m_newSetting;
	m_item->setTargetWithoutSignal(m_newSetting.graphTarget);
}

void Post2dWindowGraphGroupDataItem::SetSettingCommand::undo()
{
	m_item->impl->m_setting = m_oldSetting;
	m_item->setTargetWithoutSignal(m_oldSetting.graphTarget);
}
