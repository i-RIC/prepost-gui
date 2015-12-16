#include "post3dwindowparticlesvectorgroupdataitem_setsettingcommand.h"

Post3dWindowParticlesVectorGroupDataItem::SetSettingCommand::SetSettingCommand(const ArrowSettingContainer& s, Post3dWindowParticlesVectorGroupDataItem* item) :
	QUndoCommand {Post3dWindowParticlesVectorGroupDataItem::tr("Update Vector Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post3dWindowParticlesVectorGroupDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_item->m_setting.target);
}

void Post3dWindowParticlesVectorGroupDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->setTarget(m_item->m_setting.target);
}
