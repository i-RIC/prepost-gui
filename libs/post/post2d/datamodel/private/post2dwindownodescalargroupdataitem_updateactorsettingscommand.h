#ifndef POST2DWINDOWNODESCALARGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
#define POST2DWINDOWNODESCALARGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H

#include "../post2dwindownodescalargroupdataitem.h"

#include <QUndoCommand>

#include <memory>

class Post2dWindowNodeScalarGroupDataItem::UpdateActorSettingsCommand : public QUndoCommand
{
public:
	UpdateActorSettingsCommand(bool apply, QUndoCommand* command, Post2dWindowNodeScalarGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_apply;
	std::unique_ptr<QUndoCommand> m_command;
	Post2dWindowNodeScalarGroupDataItem* m_item;
};

#endif // POST2DWINDOWNODESCALARGROUPDATAITEM_UPDATEACTORSETTINGSCOMMAND_H
