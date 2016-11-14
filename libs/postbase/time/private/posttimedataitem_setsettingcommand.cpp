#include "posttimedataitem_setsettingcommand.h"

PostTimeDataItem::SetSettingCommand::SetSettingCommand(const PostTimeDataItem::Setting& setting, PostTimeDataItem* item) :
	QUndoCommand {PostTimeDataItem::tr("Edit time setting")},
	m_newSetting {setting},
	m_oldSetting {item->m_setting},
	m_oldEnabled {item->isEnabled()},
	m_item {item}
{}

void PostTimeDataItem::SetSettingCommand::redo()
{
	applySetting(m_newSetting, true);
}

void PostTimeDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldSetting, m_oldEnabled);
}

void PostTimeDataItem::SetSettingCommand::applySetting(const PostTimeDataItem::Setting& s, bool enabled)
{
	m_item->m_setting = s;
	m_item->setEnabled(enabled);
	m_item->updateActorSettings();
}
