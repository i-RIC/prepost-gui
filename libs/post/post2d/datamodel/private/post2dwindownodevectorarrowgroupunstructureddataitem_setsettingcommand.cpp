#include "post2dwindownodevectorarrowgroupunstructureddataitem_setsettingcommand.h"

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand::SetSettingCommand(const Post2dWindowNodeVectorArrowUnstructuredSetting& setting, const QString& scalarBarTitle, Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item) :
	QUndoCommand {Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::tr("Update Arrow Setting")},
	m_newSetting {setting},
	m_newScalarBarTitle {scalarBarTitle},
	m_oldSetting {item->m_setting},
	m_item {item}
{
	m_oldScalarBarTitle = m_item->m_colorbarTitleMap[setting.colorTarget];
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	m_item->m_colorbarTitleMap[m_newSetting.colorTarget] = m_newScalarBarTitle;
	m_item->setTarget(m_newSetting.target);
}

void Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	m_item->m_colorbarTitleMap[m_newSetting.colorTarget] = m_oldScalarBarTitle;
	m_item->setTarget(m_oldSetting.target);
}
