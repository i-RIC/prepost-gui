#include "post3dwindowparticlesbasetopdataitem_setsettingcommand.h"

Post3dWindowParticlesBaseTopDataItem::SetSettingCommand::SetSettingCommand(const PostParticleBasicSetting& s, Post3dWindowParticlesBaseTopDataItem* item) :
	QUndoCommand {Post3dWindowParticlesBaseTopDataItem::tr("Edit Particle Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post3dWindowParticlesBaseTopDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->updateActorSettings();
}

void Post3dWindowParticlesBaseTopDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->updateActorSettings();
}
