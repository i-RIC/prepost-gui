#ifndef POST2DWINDOWCELLFLAGGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWCELLFLAGGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowcellflaggroupdataitem.h"

#include <QUndoCommand>

class Post2dWindowCellFlagGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const QList<Post2dWindowCellFlagSetting>& newsettings, int newo, Post2dWindowCellFlagGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	QList<Post2dWindowCellFlagSetting> m_newSettings;
	int m_newOpacityPercent;

	QList<Post2dWindowCellFlagSetting> m_oldSettings;
	int m_oldOpacityPercent;
	Post2dWindowCellFlagGroupDataItem* m_item;
};

#endif // POST2DWINDOWCELLFLAGGROUPDATAITEM_SETSETTINGCOMMAND_H
