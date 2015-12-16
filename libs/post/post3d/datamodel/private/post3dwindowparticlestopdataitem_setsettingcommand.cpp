#include "post3dwindowparticlestopdataitem_setsettingcommand.h"

Post3dWindowParticlesTopDataItem::SetSettingCommand::SetSettingCommand(const PostParticleBasicSetting& s, Post3dWindowParticlesTopDataItem* item) :
	QUndoCommand {Post3dWindowParticlesTopDataItem::tr("Edit Particle Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post3dWindowParticlesTopDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->updateActorSettings();
}

void Post3dWindowParticlesTopDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->updateActorSettings();
}
