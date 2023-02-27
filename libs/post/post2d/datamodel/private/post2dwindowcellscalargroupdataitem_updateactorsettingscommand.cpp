#include "post2dwindowcellscalargroupdataitem_updateactorsettingscommand.h"

#include <misc/qundocommandhelper.h>

Post2dWindowCellScalarGroupDataItem::UpdateActorSettingsCommand::UpdateActorSettingsCommand(bool apply, QUndoCommand* command, Post2dWindowCellScalarGroupDataItem* item) :
	m_apply {apply},
	m_command {command},
	m_item {item}
{}

void Post2dWindowCellScalarGroupDataItem::UpdateActorSettingsCommand::redo()
{
	m_command->redo();;
	m_item->updateActorSettings();
}

void Post2dWindowCellScalarGroupDataItem::UpdateActorSettingsCommand::undo()
{
	m_command->undo();
	m_item->updateActorSettings();
}

int Post2dWindowCellScalarGroupDataItem::UpdateActorSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post2dWindowCellScalarGroupDataItem::UpdateActorSettingsCommand");
}

bool Post2dWindowCellScalarGroupDataItem::UpdateActorSettingsCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto c = dynamic_cast<const UpdateActorSettingsCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
