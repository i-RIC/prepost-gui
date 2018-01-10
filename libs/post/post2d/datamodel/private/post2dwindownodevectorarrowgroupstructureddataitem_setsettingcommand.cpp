#include "post2dwindownodevectorarrowgroupstructureddataitem_setsettingcommand.h"

Post2dWindowNodeVectorArrowGroupStructuredDataItem::SetSettingCommand::SetSettingCommand(const Post2dWindowNodeVectorArrowStructuredSetting& setting, const QString& colorBarTitle, Post2dWindowNodeVectorArrowGroupStructuredDataItem* item) :
	QUndoCommand(Post2dWindowNodeVectorArrowGroupStructuredDataItem::tr("Update Arrow Setting")),
	m_newSetting {setting},
	m_newScalarBarTitle {colorBarTitle},
	m_oldSetting {item->m_setting},
	m_item {item}
{
	m_oldScalarBarTitle = m_item->m_colorbarTitleMap[setting.colorTarget];
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->m_colorbarTitleMap[m_newSetting.colorTarget] = m_newScalarBarTitle;
	m_item->setTarget(m_newSetting.target);
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->m_colorbarTitleMap[m_newSetting.colorTarget] = m_oldScalarBarTitle;
	m_item->setTarget(m_oldSetting.target);
}
