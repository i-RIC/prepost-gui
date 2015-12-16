#include "post2dwindowparticlestopdataitem_setsettingcommand.h"

Post2dWindowParticlesTopDataItem::SetSettingCommand::SetSettingCommand(const PostParticleBasicSetting& s, Post2dWindowParticlesTopDataItem* item) :
	QUndoCommand {Post2dWindowParticlesTopDataItem::tr("Edit Particle Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post2dWindowParticlesTopDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->updateActorSettings();
}

void Post2dWindowParticlesTopDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->updateActorSettings();
}
