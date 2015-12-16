#ifndef POST3DWINDOWARROWGROUPDATAITEM_UPDATEONREDOCOMMAND_H
#define POST3DWINDOWARROWGROUPDATAITEM_UPDATEONREDOCOMMAND_H

#include "../post3dwindowarrowgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowArrowGroupDataItem::UpdateOnRedoCommand : public QUndoCommand
{
public:
	UpdateOnRedoCommand(Post3dWindowArrowGroupDataItem* item);
	void redo() override;
	void undo() override;

private:
	Post3dWindowArrowGroupDataItem* m_item;
};

#endif // POST3DWINDOWARROWGROUPDATAITEM_UPDATEONREDOCOMMAND_H
