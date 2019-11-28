#include "post2dwindowparticlesbasevectorgroupdataitem_setsettingcommand.h"

Post2dWindowParticlesBaseVectorGroupDataItem::SetSettingCommand::SetSettingCommand(const ArrowSettingContainer& s, Post2dWindowParticlesBaseVectorGroupDataItem* item) :
	QUndoCommand {Post2dWindowParticlesBaseVectorGroupDataItem::tr("Update Vector Setting")},
	m_newSetting {s},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post2dWindowParticlesBaseVectorGroupDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_item->m_setting.target);
}

void Post2dWindowParticlesBaseVectorGroupDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->setTarget(m_item->m_setting.target);
}
