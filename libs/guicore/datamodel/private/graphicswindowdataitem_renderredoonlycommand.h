#ifndef GRAPHICSWINDOWDATAITEM_RENDERREDOONLYCOMMAND_H
#define GRAPHICSWINDOWDATAITEM_RENDERREDOONLYCOMMAND_H

#include "../graphicswindowdataitem.h"

#include <QUndoCommand>

#include <memory>

class GraphicsWindowDataItem::RenderRedoOnlyCommand : public QUndoCommand
{
public:
	explicit RenderRedoOnlyCommand(QUndoCommand *child, GraphicsWindowDataItem* item);
	~RenderRedoOnlyCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEM_RENDERREDOONLYCOMMAND_H
