#ifndef POST2DWINDOWCELLFLAGGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWCELLFLAGGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowcellflaggroupdataitem.h"

#include <misc/opacitycontainer.h>

#include <QUndoCommand>

class Post2dWindowCellFlagGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const QList<Post2dWindowCellFlagSetting>& newsettings, const OpacityContainer& opacity, Post2dWindowCellFlagGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	QList<Post2dWindowCellFlagSetting> m_newSettings;
	OpacityContainer m_newOpacity;

	QList<Post2dWindowCellFlagSetting> m_oldSettings;
	OpacityContainer m_oldOpacity;

	Post2dWindowCellFlagGroupDataItem* m_item;
};

#endif // POST2DWINDOWCELLFLAGGROUPDATAITEM_SETSETTINGCOMMAND_H
