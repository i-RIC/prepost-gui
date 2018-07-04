#include "graphicswindowdataitem_renderredoonlycommand.h"

GraphicsWindowDataItem::RenderRedoOnlyCommand::RenderRedoOnlyCommand(QUndoCommand *child, GraphicsWindowDataItem* item) :
	QUndoCommand {child->text()},
	m_command {child},
	m_item {item}
{}

GraphicsWindowDataItem::RenderRedoOnlyCommand::~RenderRedoOnlyCommand()
{}

void GraphicsWindowDataItem::RenderRedoOnlyCommand::redo()
{
	m_command.get()->redo();
	m_item->renderGraphicsView();
}

void GraphicsWindowDataItem::RenderRedoOnlyCommand::undo()
{
	m_command.get()->undo();
}

int GraphicsWindowDataItem::RenderRedoOnlyCommand::id() const
{
	return m_command->id();
}

bool GraphicsWindowDataItem::RenderRedoOnlyCommand::mergeWith(const QUndoCommand *other)
{
	const RenderRedoOnlyCommand* modc = dynamic_cast<const RenderRedoOnlyCommand*> (other);
	if (modc == nullptr) {return false;}
	return m_command.get()->mergeWith(modc->m_command.get());
}
