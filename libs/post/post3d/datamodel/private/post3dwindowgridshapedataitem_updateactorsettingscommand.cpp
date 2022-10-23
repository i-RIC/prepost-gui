#include "post3dwindowgridshapedataitem_updateactorsettingscommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowGridShapeDataItem::UpdateActorSettingsCommand::UpdateActorSettingsCommand(bool apply, QUndoCommand* command, Post3dWindowGridShapeDataItem* item) :
	m_apply {apply},
	m_command {command},
	m_item {item}
{}

void Post3dWindowGridShapeDataItem::UpdateActorSettingsCommand::redo()
{
	m_command->redo();
	m_item->updateActorSettings();
}

void Post3dWindowGridShapeDataItem::UpdateActorSettingsCommand::undo()
{
	m_command->undo();
	m_item->updateActorSettings();
}

int Post3dWindowGridShapeDataItem::UpdateActorSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowGridShapeDataItem::UpdateActorSettingsCommand");
}

bool Post3dWindowGridShapeDataItem::UpdateActorSettingsCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}
	auto other2 = dynamic_cast<const UpdateActorSettingsCommand*> (other);
	if (other2 == nullptr) {return false;}

	bool ok = m_command->mergeWith(other2->m_command.get());
	if (! ok) {return false;}

	m_apply = other2->m_apply;
	return true;
}
