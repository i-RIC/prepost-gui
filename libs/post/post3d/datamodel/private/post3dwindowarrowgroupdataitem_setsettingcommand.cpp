#include "post3dwindowarrowgroupdataitem_setsettingcommand.h"

#include <misc/qttool.h>

#include <QStandardItem>

Post3dWindowArrowGroupDataItem::SetSettingCommand::SetSettingCommand(const ArrowSettingContainer& setting, Post3dWindowArrowGroupDataItem* g) :
	QUndoCommand(),
	m_newEnabled {true},
	m_newSetting {setting},
	m_oldEnabled {(g->standardItem()->checkState() == Qt::Checked)},
	m_oldSetting {g->m_setting},
	m_item {g}
{}

void Post3dWindowArrowGroupDataItem::SetSettingCommand::redo()
{
	m_item->m_isCommandExecuting = true;

	QtTool::checkItem(m_item->standardItem(), m_newEnabled);
	m_item->m_setting = m_newSetting;

	m_item->m_isCommandExecuting = false;
}

void Post3dWindowArrowGroupDataItem::SetSettingCommand::undo()
{
	m_item->m_isCommandExecuting = true;

	QtTool::checkItem(m_item->standardItem(), m_oldEnabled);
	m_item->m_setting = m_oldSetting;

	m_item->m_isCommandExecuting = false;
}
