#ifndef GRAPHICSWINDOWDATAITEM_STANDARDITEMMODIFYCOMMAND_H
#define GRAPHICSWINDOWDATAITEM_STANDARDITEMMODIFYCOMMAND_H

#include "../graphicswindowdataitem.h"

#include <QUndoCommand>

#include <memory>

class GraphicsWindowDataItem::StandardItemModifyCommand : public QUndoCommand
{
public:
	StandardItemModifyCommand(QUndoCommand *child, GraphicsWindowDataItem* item);
	~StandardItemModifyCommand();

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEM_STANDARDITEMMODIFYCOMMAND_H
