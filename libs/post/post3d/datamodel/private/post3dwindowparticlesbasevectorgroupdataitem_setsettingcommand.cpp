#include "post3dwindowparticlesbasevectorgroupdataitem_setsettingcommand.h"

Post3dWindowParticlesBaseVectorGroupDataItem::SetSettingCommand::SetSettingCommand(const ArrowSettingContainer& s, Post3dWindowParticlesBaseVectorGroupDataItem* item) :
	QUndoCommand {Post3dWindowParticlesBaseVectorGroupDataItem::tr("Update Vector Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post3dWindowParticlesBaseVectorGroupDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_item->m_setting.target);
}

void Post3dWindowParticlesBaseVectorGroupDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->setTarget(m_item->m_setting.target);
}
