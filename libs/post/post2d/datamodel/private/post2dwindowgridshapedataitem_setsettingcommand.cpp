#include "post2dwindowgridshapedataitem_setsettingcommand.h"

Post2dWindowGridShapeDataItem::SetSettingCommand::SetSettingCommand(const GridShapeEditDialog::Setting& setting, Post2dWindowGridShapeDataItem* item) :
	QUndoCommand {QObject::tr("Update Grid Shape Setting")},
	m_newSetting {setting},
	m_oldSetting {item->m_setting},
	m_oldEnabled {item->isEnabled()},
	m_item {item}
{}

void Post2dWindowGridShapeDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->setEnabled(true);
	m_item->updateActorSettings();
}

void Post2dWindowGridShapeDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->setEnabled(m_oldEnabled);
	m_item->updateActorSettings();
}
