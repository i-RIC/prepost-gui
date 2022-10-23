#include "post3dwindownodevectorarrowgroupdataitem_updateactorsettingscommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::UpdateActorSettingsCommand(bool apply, QUndoCommand* updateArrowsCommand, QUndoCommand* updateSettingsCommand, QUndoCommand* updateColormapsCommand, Post3dWindowNodeVectorArrowGroupDataItem* item) :
	QUndoCommand {},
	m_apply {apply},
	m_updateArrowsCommand {updateArrowsCommand},
	m_updateSettingsCommand {updateSettingsCommand},
	m_updateColormapsCommand {updateColormapsCommand},
	m_item {item}
{}

void Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::redo()
{
	m_updateArrowsCommand->redo();
	m_updateSettingsCommand->redo();
	m_updateColormapsCommand->redo();

	m_item->updateActorSettings();
}

void Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::undo()
{
	m_updateArrowsCommand->undo();
	m_updateSettingsCommand->undo();
	m_updateColormapsCommand->undo();

	m_item->updateActorSettings();
}

int Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand");
}

bool Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = dynamic_cast<const UpdateActorSettingsCommand*> (other);
	if (other2 == nullptr) {return false;}

	m_updateArrowsCommand->mergeWith(other2->m_updateArrowsCommand);
	m_updateSettingsCommand->mergeWith(other2->m_updateSettingsCommand);
	m_updateColormapsCommand->mergeWith(other2->m_updateColormapsCommand);

	m_apply = other2->m_apply;
	return true;
}
