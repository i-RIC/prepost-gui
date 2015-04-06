#include "graphicswindowdrawcommands.h"
#include "graphicswindowdataitem.h"

GraphicsWindowDrawOnRedo::GraphicsWindowDrawOnRedo(GraphicsWindowDataItem* item)
	: QUndoCommand(QObject::tr("Draw"))
{
	m_item = item;
}

void GraphicsWindowDrawOnRedo::redo()
{
	m_item->renderGraphicsView();
}

GraphicsWindowDrawOnUndo::GraphicsWindowDrawOnUndo(GraphicsWindowDataItem* item)
	: QUndoCommand(QObject::tr("Draw"))
{
	m_item = item;
}

void GraphicsWindowDrawOnUndo::undo()
{
	m_item->renderGraphicsView();
}
