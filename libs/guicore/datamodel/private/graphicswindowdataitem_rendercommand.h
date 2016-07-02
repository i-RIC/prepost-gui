#ifndef GRAPHICSWINDOWDATAITEM_RENDERCOMMAND_H
#define GRAPHICSWINDOWDATAITEM_RENDERCOMMAND_H

#include "../graphicswindowdataitem.h"

#include <QUndoCommand>

#include <memory>

class GraphicsWindowDataItem::RenderCommand : public QUndoCommand
{
public:
	explicit RenderCommand(QUndoCommand *child, GraphicsWindowDataItem* item);
	~RenderCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEM_RENDERCOMMAND_H
