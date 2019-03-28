#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_SETBASICSETTINGCOMMAND_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_SETBASICSETTINGCOMMAND_H

#include "../post2dwindowpolydatagroupdataitem.h"

#include <QUndoCommand>

class Post2dWindowPolyDataGroupDataItem::SetBasicSettingCommand : public QUndoCommand
{
public:
	SetBasicSettingCommand(const PostPolyDataBasicSetting& s, Post2dWindowPolyDataGroupDataItem* item);

	void redo() override;
	void undo() override;

private:
	PostPolyDataBasicSetting m_newSetting;
	PostPolyDataBasicSetting m_oldSetting;

	Post2dWindowPolyDataGroupDataItem* m_item;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_SETBASICSETTINGCOMMAND_H
