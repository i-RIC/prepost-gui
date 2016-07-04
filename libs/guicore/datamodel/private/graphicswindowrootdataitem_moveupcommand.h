#ifndef GRAPHICSWINDOWROOTDATAITEM_MOVEUPCOMMAND_H
#define GRAPHICSWINDOWROOTDATAITEM_MOVEUPCOMMAND_H

#include "../graphicswindowrootdataitem.h"

#include <QUndoCommand>

class GraphicsWindowRootDataItem::MoveUpCommand : public QUndoCommand
{
public:
	MoveUpCommand(GraphicsWindowDataItem* item, ObjectBrowserView* view);

	void redo() override;
	void undo() override;

private:
	GraphicsWindowDataItem* m_item;
	ObjectBrowserView* m_view;
};

#endif // GRAPHICSWINDOWROOTDATAITEM_MOVEUPCOMMAND_H
