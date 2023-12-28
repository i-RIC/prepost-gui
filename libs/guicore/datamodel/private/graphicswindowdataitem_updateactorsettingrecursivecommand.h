#ifndef GRAPHICSWINDOWDATAITEM_UPDATEACTORSETTINGRECURSIVECOMMAND_H
#define GRAPHICSWINDOWDATAITEM_UPDATEACTORSETTINGRECURSIVECOMMAND_H

#include "../graphicswindowdataitem.h"

#include <QUndoCommand>

#include <memory>

class GraphicsWindowDataItem::UpdateActorSettingRecursiveCommand : public QUndoCommand
{
public:
	UpdateActorSettingRecursiveCommand(QUndoCommand* child, GraphicsWindowDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEM_UPDATEACTORSETTINGRECURSIVECOMMAND_H
