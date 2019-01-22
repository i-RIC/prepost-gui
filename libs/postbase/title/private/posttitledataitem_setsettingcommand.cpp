#include "posttitledataitem_setsettingcommand.h"

PostTitleDataItem::SetSettingCommand::SetSettingCommand(bool enable, const PostTitleSetting& setting, PostTitleDataItem* item) :
	m_newEnabled {enable},
	m_newSetting {setting},
	m_oldEnabled {item->isEnabled()},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void PostTitleDataItem::SetSettingCommand::redo()
{
	m_item->setEnabled(m_newEnabled);
	m_item->m_setting = m_newSetting;
	m_item->updateActorSettings();
}

void PostTitleDataItem::SetSettingCommand::undo()
{
	m_item->setEnabled(m_oldEnabled);
	m_item->m_setting = m_oldSetting;
	m_item->updateActorSettings();
}
