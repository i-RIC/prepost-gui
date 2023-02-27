#include "post2dwindownodescalargroupdataitem_updateactorsettingscommand.h"

#include <misc/qundocommandhelper.h>

Post2dWindowNodeScalarGroupDataItem::UpdateActorSettingsCommand::UpdateActorSettingsCommand(bool apply, QUndoCommand* command, Post2dWindowNodeScalarGroupDataItem* item) :
	m_apply {apply},
	m_command {command},
	m_item {item}
{}

void Post2dWindowNodeScalarGroupDataItem::UpdateActorSettingsCommand::redo()
{
	m_command->redo();;
	m_item->updateActorSettings();
}

void Post2dWindowNodeScalarGroupDataItem::UpdateActorSettingsCommand::undo()
{
	m_command->undo();
	m_item->updateActorSettings();
}

int Post2dWindowNodeScalarGroupDataItem::UpdateActorSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post2dWindowNodeScalarGroupDataItem::UpdateActorSettingsCommand");
}

bool Post2dWindowNodeScalarGroupDataItem::UpdateActorSettingsCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto c = dynamic_cast<const UpdateActorSettingsCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
