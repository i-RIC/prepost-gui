#include "post2dwindowcellflaggroupdataitem_setsettingcommand.h"

Post2dWindowCellFlagGroupDataItem::SetSettingCommand::SetSettingCommand(const QList<Post2dWindowCellFlagSetting>& newsettings, const OpacityContainer& opacity, Post2dWindowCellFlagGroupDataItem* item) :
	QUndoCommand(Post2dWindowCellFlagGroupDataItem::tr("Cell Flag Setting"))
{
	m_newSettings = newsettings;
	m_newOpacity = opacity;

	m_oldSettings = item->settings();
	m_oldOpacity = item->m_opacity;
	m_item = item;
}

void Post2dWindowCellFlagGroupDataItem::SetSettingCommand::redo()
{
	m_item->setSettings(m_newSettings, m_newOpacity);
	m_item->renderGraphicsView();
}

void Post2dWindowCellFlagGroupDataItem::SetSettingCommand::undo()
{
	m_item->setSettings(m_oldSettings, m_oldOpacity);
	m_item->renderGraphicsView();
}
