#ifndef GRAPHICSWINDOWDATAITEM_UPDATEACTORSETTINGCOMMAND_H
#define GRAPHICSWINDOWDATAITEM_UPDATEACTORSETTINGCOMMAND_H

#include "../graphicswindowdataitem.h"

#include <QUndoCommand>

#include <memory>

class GraphicsWindowDataItem::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(QUndoCommand* child, GraphicsWindowDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand *other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GraphicsWindowDataItem* m_item;
};

#endif // GRAPHICSWINDOWDATAITEM_UPDATEACTORSETTINGCOMMAND_H
