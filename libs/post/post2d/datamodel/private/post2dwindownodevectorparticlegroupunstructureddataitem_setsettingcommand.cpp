#include "post2dwindownodevectorparticlegroupunstructureddataitem_setsettingcommand.h"

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SetSettingCommand::SetSettingCommand(const Post2dWindowNodeVectorParticleGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting>& unsts, Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* item) :
	QUndoCommand(Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::tr("Update Particle Setting")),
	m_newSetting {s},
	m_newUnstSettings (unsts),
	m_oldEnabled {item->isEnabled()},
	m_oldSetting {item->m_setting},
	m_oldUnstSettings (item->m_unstSettings),
	m_item {item}
{}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SetSettingCommand::redo()
{
	m_item->setEnabled(true);
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_newSetting.target);
	m_item->m_unstSettings = m_newUnstSettings;
	m_item->updateActorSettings();
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SetSettingCommand::undo()
{
	m_item->setEnabled(m_oldEnabled);
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_newSetting.target);
	m_item->m_unstSettings = m_newUnstSettings;
	m_item->updateActorSettings();
}
