#include "post2dwindowparticlesvectorgroupdataitem_setsettingcommand.h"

Post2dWindowParticlesVectorGroupDataItem::SetSettingCommand::SetSettingCommand(const ArrowSettingContainer& s, Post2dWindowParticlesVectorGroupDataItem* item) :
	QUndoCommand {Post2dWindowParticlesVectorGroupDataItem::tr("Update Vector Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post2dWindowParticlesVectorGroupDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_item->m_setting.target);
}

void Post2dWindowParticlesVectorGroupDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->setTarget(m_item->m_setting.target);
}
