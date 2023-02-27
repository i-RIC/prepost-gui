#include "post3dwindowcontourgroupdataitem_updateactorsettingscommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowContourGroupDataItem::UpdateActorSettingsCommand::UpdateActorSettingsCommand(bool apply, QUndoCommand* updateColorMapCommand, QUndoCommand* updateFacesCommand, Post3dWindowContourGroupDataItem* item) :
	QUndoCommand {},
	m_apply {apply},
	m_updateColorMapCommand {updateColorMapCommand},
	m_updateFacesCommand {updateFacesCommand},
	m_item {item}
{}

void Post3dWindowContourGroupDataItem::UpdateActorSettingsCommand::redo()
{
	m_updateColorMapCommand->redo();
	m_updateFacesCommand->redo();

	m_item->updateActorSettings();
}

void Post3dWindowContourGroupDataItem::UpdateActorSettingsCommand::undo()
{
	m_updateColorMapCommand->undo();
	m_updateFacesCommand->undo();

	m_item->updateActorSettings();
}

int Post3dWindowContourGroupDataItem::UpdateActorSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowContourGroupDataItem::UpdateActorSettingsCommand");
}

bool Post3dWindowContourGroupDataItem::UpdateActorSettingsCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = dynamic_cast<const UpdateActorSettingsCommand*> (other);
	if (other2 == nullptr) {return false;}

	m_updateColorMapCommand->mergeWith(other2->m_updateColorMapCommand);
	m_updateFacesCommand->mergeWith(other2->m_updateFacesCommand);

	m_apply = other2->m_apply;
	return true;
}
