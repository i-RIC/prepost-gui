#ifndef GRIDBIRDEYEWINDOWGRIDDATAITEM_UPDATEACTORSETTINGCOMMAND_H
#define GRIDBIRDEYEWINDOWGRIDDATAITEM_UPDATEACTORSETTINGCOMMAND_H

#include "../gridbirdeyewindowgriddataitem.h"

#include <QUndoCommand>

#include <memory>

class GridBirdEyeWindowGridDataItem::UpdateActorSettingCommand : public QUndoCommand
{
public:
	UpdateActorSettingCommand(QUndoCommand* command, GridBirdEyeWindowGridDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	std::unique_ptr<QUndoCommand> m_command;
	GridBirdEyeWindowGridDataItem* m_item;
};

#endif // GRIDBIRDEYEWINDOWGRIDDATAITEM_UPDATEACTORSETTINGCOMMAND_H
