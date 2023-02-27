#ifndef POST2DWINDOWCELLSCALARGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
#define POST2DWINDOWCELLSCALARGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H

#include "../post2dwindowcellscalargroupdataitem.h"

#include <QUndoCommand>

#include <memory>

class Post2dWindowCellScalarGroupDataItem::UpdateActorSettingsCommand : public QUndoCommand
{
public:
	UpdateActorSettingsCommand(bool apply, QUndoCommand* command, Post2dWindowCellScalarGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;
	std::unique_ptr<QUndoCommand> m_command;
	Post2dWindowCellScalarGroupDataItem* m_item;
};

#endif // POST2DWINDOWCELLSCALARGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
