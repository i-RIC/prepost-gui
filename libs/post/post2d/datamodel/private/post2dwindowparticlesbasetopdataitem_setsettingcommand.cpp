#include "post2dwindowparticlesbasetopdataitem_setsettingcommand.h"

Post2dWindowParticlesBaseTopDataItem::SetSettingCommand::SetSettingCommand(const PostParticleBasicSetting& s, Post2dWindowParticlesBaseTopDataItem* item) :
	QUndoCommand {Post2dWindowParticlesBaseTopDataItem::tr("Edit Particle Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post2dWindowParticlesBaseTopDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->updateActorSettings();
}

void Post2dWindowParticlesBaseTopDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->updateActorSettings();
}
