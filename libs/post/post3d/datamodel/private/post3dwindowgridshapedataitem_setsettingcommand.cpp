#include "post3dwindowgridshapedataitem_setsettingcommand.h"

Post3dWindowGridShapeDataItem::SetSettingCommand::SetSettingCommand(const GridShapeEditDialog::Setting& setting, Post3dWindowGridShapeDataItem* item) :
	QUndoCommand(Post3dWindowGridShapeDataItem::tr("Update Grid Shape Setting")),
	m_newSetting {setting},
	m_oldSetting {item->m_setting},
	m_oldEnabled {item->isEnabled()},
	m_item {item}
{}

void Post3dWindowGridShapeDataItem::SetSettingCommand::redo()
{
	apply(m_newSetting, true);
}

void Post3dWindowGridShapeDataItem::SetSettingCommand::undo()
{
	apply(m_oldSetting, m_oldEnabled);
}

void Post3dWindowGridShapeDataItem::SetSettingCommand::apply(const GridShapeEditDialog::Setting& setting, bool enabled)
{
	m_item->setIsCommandExecuting(true);
	m_item->m_setting = setting;
	m_item->setEnabled(enabled);

	m_item->updateActorSettings();
	m_item->setIsCommandExecuting(false);
}
