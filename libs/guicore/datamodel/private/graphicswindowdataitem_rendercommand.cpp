#include "graphicswindowdataitem_rendercommand.h"

GraphicsWindowDataItem::RenderCommand::RenderCommand(QUndoCommand *child, GraphicsWindowDataItem* item) :
	QUndoCommand {child->text()},
	m_command {child},
	m_item {item}
{}

void GraphicsWindowDataItem::RenderCommand::redo()
{
	m_command.get()->redo();
	m_item->renderGraphicsView();
}

void GraphicsWindowDataItem::RenderCommand::undo()
{
	m_command.get()->undo();
	m_item->renderGraphicsView();
}

int GraphicsWindowDataItem::RenderCommand::id() const
{
	return m_command->id();
}

bool GraphicsWindowDataItem::RenderCommand::mergeWith(const QUndoCommand *other)
{
	const RenderCommand* modc = dynamic_cast<const RenderCommand*> (other);
	if (modc == nullptr) {return false;}

	return m_command.get()->mergeWith(modc->m_command.get());
}
