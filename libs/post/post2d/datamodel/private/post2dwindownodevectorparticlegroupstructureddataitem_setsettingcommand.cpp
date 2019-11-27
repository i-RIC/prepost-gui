#include "post2dwindownodevectorparticlegroupstructureddataitem_setsettingcommand.h"

Post2dWindowNodeVectorParticleGroupStructuredDataItem::SetSettingCommand::SetSettingCommand(const Post2dWindowNodeVectorParticleGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting>& settings, Post2dWindowNodeVectorParticleGroupStructuredDataItem* item) :
	QUndoCommand (Post2dWindowNodeVectorParticleGroupStructuredDataItem::tr("Update Particle Setting")),
	m_newSetting {s},
	m_newStSettings (settings),
	m_oldEnabled {item->isEnabled()},
	m_oldSetting {item->m_setting},
	m_oldStSettings (item->m_stSettings),
	m_item {item}
{}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::SetSettingCommand::redo() {
	m_item->setEnabled(true);
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_newSetting.target);
	m_item->m_stSettings = m_newStSettings;
	m_item->updateActorSettings();
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::SetSettingCommand::undo() {
	m_item->setEnabled(m_oldEnabled);
	m_item->m_setting = m_oldSetting;
	m_item->setTarget(m_oldSetting.target);
	m_item->m_stSettings = m_oldStSettings;
	m_item->updateActorSettings();
}
