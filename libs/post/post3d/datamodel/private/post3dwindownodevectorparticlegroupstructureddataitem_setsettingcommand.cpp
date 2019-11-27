#include "post3dwindownodevectorparticlegroupstructureddataitem_setsettingcommand.h"

Post3dWindowNodeVectorParticleGroupStructuredDataItem::SetSettingCommand::SetSettingCommand(const Post3dWindowNodeVectorParticleGroupDataItem::Setting& setting, const QList<Post3dWindowStructuredParticleSetSetting>& settings, Post3dWindowNodeVectorParticleGroupStructuredDataItem* item) :
	QUndoCommand(Post3dWindowNodeVectorParticleGroupStructuredDataItem::tr("Update Particle Setting")),
	m_newSetting {setting},
	m_newSettings (settings),
	m_oldSetting {item->m_setting},
	m_oldSettings (item->m_settings),
	m_oldEnabled {item->isEnabled()},
	m_item {item}
{}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SetSettingCommand::redo()
{
	m_item->setIsCommandExecuting(true);

	m_item->m_setting = m_newSetting;
	m_item->m_settings = m_newSettings;
	m_item->setEnabled(true);
	m_item->setTarget(m_newSetting.target);
	m_item->updateActorSettings();

	m_item->setIsCommandExecuting(false);
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SetSettingCommand::undo()
{
	m_item->setIsCommandExecuting(true);

	m_item->m_setting = m_oldSetting;
	m_item->m_settings = m_oldSettings;
	m_item->setEnabled(m_oldEnabled);
	m_item->setTarget(m_oldSetting.target);
	m_item->updateActorSettings();

	m_item->setIsCommandExecuting(false);
}
