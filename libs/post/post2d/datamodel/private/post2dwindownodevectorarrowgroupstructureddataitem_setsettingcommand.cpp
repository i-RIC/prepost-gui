#include "post2dwindownodevectorarrowgroupstructureddataitem_setsettingcommand.h"

Post2dWindowNodeVectorArrowGroupStructuredDataItem::SetSettingCommand::SetSettingCommand(const Post2dWindowNodeVectorArrowStructuredSetting& setting, Post2dWindowNodeVectorArrowGroupStructuredDataItem* item) :
	QUndoCommand(Post2dWindowNodeVectorArrowGroupStructuredDataItem::tr("Update Arrow Setting")),
	m_newSetting {setting},
	m_oldSetting {item->m_setting},
	m_item {item}
{}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->setTarget(m_newSetting.target);
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->setTarget(m_oldSetting.target);
}
