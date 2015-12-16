#ifndef POST3DWINDOWARROWGROUPDATAITEM_UPDATEONUNDOCOMMAND_H
#define POST3DWINDOWARROWGROUPDATAITEM_UPDATEONUNDOCOMMAND_H

#include "../post3dwindowarrowgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowArrowGroupDataItem::UpdateOnUndoCommand : public QUndoCommand
{
public:
	UpdateOnUndoCommand(Post3dWindowArrowGroupDataItem* item);
	void redo() override;
	void undo() override;

private:
	Post3dWindowArrowGroupDataItem* m_item;
};

#endif // POST3DWINDOWARROWGROUPDATAITEM_UPDATEONUNDOCOMMAND_H
