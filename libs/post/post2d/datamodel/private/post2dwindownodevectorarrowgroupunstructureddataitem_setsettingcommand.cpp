#include "post2dwindownodevectorarrowgroupunstructureddataitem_setsettingcommand.h"

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand::SetSettingCommand(const Post2dWindowNodeVectorArrowUnstructuredSetting& setting, Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item) :
	QUndoCommand {Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::tr("Update Arrow Setting")},
	m_newSetting {setting},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_newSetting.target);
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->setTarget(m_oldSetting.target);
}
