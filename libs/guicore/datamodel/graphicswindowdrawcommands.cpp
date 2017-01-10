#include "graphicswindowdataitem.h"
#include "graphicswindowdrawcommands.h"

GraphicsWindowDrawOnRedo::GraphicsWindowDrawOnRedo(GraphicsWindowDataItem* item, QUndoCommand* parent)
	: QUndoCommand(QObject::tr("Draw"), parent)
{
	m_item = item;
}

void GraphicsWindowDrawOnRedo::redo()
{
	m_item->renderGraphicsView();
}

GraphicsWindowDrawOnUndo::GraphicsWindowDrawOnUndo(GraphicsWindowDataItem* item, QUndoCommand* parent)
	: QUndoCommand(QObject::tr("Draw"), parent)
{
	m_item = item;
}

void GraphicsWindowDrawOnUndo::undo()
{
	m_item->renderGraphicsView();
}
