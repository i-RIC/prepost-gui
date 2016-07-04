#include "post2dwindowcellflaggroupdataitem_setsettingcommand.h"

Post2dWindowCellFlagGroupDataItem::SetSettingCommand::SetSettingCommand(const QList<Post2dWindowCellFlagSetting>& newsettings, int newo, Post2dWindowCellFlagGroupDataItem* item) :
	QUndoCommand(Post2dWindowCellFlagGroupDataItem::tr("Cell Flag Setting"))
{
	m_newSettings = newsettings;
	m_newOpacityPercent = newo;

	m_oldSettings = item->settings();
	m_oldOpacityPercent = item->m_opacityPercent;
	m_item = item;
}

void Post2dWindowCellFlagGroupDataItem::SetSettingCommand::redo()
{
	m_item->setSettings(m_newSettings, m_newOpacityPercent);
	m_item->renderGraphicsView();
}

void Post2dWindowCellFlagGroupDataItem::SetSettingCommand::undo()
{
	m_item->setSettings(m_oldSettings, m_oldOpacityPercent);
	m_item->renderGraphicsView();
}
