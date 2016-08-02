#ifndef POST2DWINDOWGRAPHGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWGRAPHGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindowgraphgroupdataitem.h"
#include "../post2dwindowgraphsetting.h"

#include <QUndoCommand>

class Post2dWindowGraphGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowGraphSetting& newSetting, Post2dWindowGraphGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	Post2dWindowGraphSetting m_newSetting;
	Post2dWindowGraphSetting m_oldSetting;

	Post2dWindowGraphGroupDataItem* m_item;
};

#endif // POST2DWINDOWGRAPHGROUPDATAITEM_SETSETTINGCOMMAND_H
