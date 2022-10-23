#include "post2dwindownodevectorarrowgroupdataitem_updateactorsettingscommand.h"

#include <misc/qundocommandhelper.h>

Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::UpdateActorSettingsCommand(bool apply, QUndoCommand* command, Post2dWindowNodeVectorArrowGroupDataItem* item) :
	m_apply {apply},
	m_command {command},
	m_item {item}
{}

void Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::redo()
{
	m_command->redo();
	m_item->updateCheckState();
	m_item->updateActorSettings();
}

void Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::undo()
{
	m_command->undo();
	m_item->updateCheckState();
	m_item->updateActorSettings();
}

int Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand");
}

bool Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto c = dynamic_cast<const UpdateActorSettingsCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
