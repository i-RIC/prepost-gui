#ifndef GRAPHICSWINDOWROOTDATAITEM_MOVEDOWNCOMMAND_H
#define GRAPHICSWINDOWROOTDATAITEM_MOVEDOWNCOMMAND_H

#include "../graphicswindowrootdataitem.h"

#include <QUndoCommand>

class GraphicsWindowRootDataItem::MoveDownCommand : public QUndoCommand
{
public:
	MoveDownCommand(GraphicsWindowDataItem* item, ObjectBrowserView* view);

	void redo() override;
	void undo() override;

private:
	GraphicsWindowDataItem* m_item;
	ObjectBrowserView* m_view;
};

#endif // GRAPHICSWINDOWROOTDATAITEM_MOVEDOWNCOMMAND_H
