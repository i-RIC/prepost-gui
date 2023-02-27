#include "post3dwindowcellcontourgroupdataitem_updateactorsettingscommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowCellContourGroupDataItem::UpdateActorSettingsCommand::UpdateActorSettingsCommand(bool apply, QUndoCommand* updateColorMapCommand, QUndoCommand* updateRangesCommand, Post3dWindowCellContourGroupDataItem* item) :
	QUndoCommand {},
	m_apply {apply},
	m_updateColorMapCommand {updateColorMapCommand},
	m_updateRangesCommand {updateRangesCommand},
	m_item {item}
{}

void Post3dWindowCellContourGroupDataItem::UpdateActorSettingsCommand::redo()
{
	m_updateColorMapCommand->redo();
	m_updateRangesCommand->redo();

	m_item->updateActorSettings();
}

void Post3dWindowCellContourGroupDataItem::UpdateActorSettingsCommand::undo()
{
	m_updateColorMapCommand->undo();
	m_updateRangesCommand->undo();

	m_item->updateActorSettings();
}

int Post3dWindowCellContourGroupDataItem::UpdateActorSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowCellContourGroupDataItem::UpdateActorSettingsCommand");
}

bool Post3dWindowCellContourGroupDataItem::UpdateActorSettingsCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = dynamic_cast<const UpdateActorSettingsCommand*> (other);
	if (other2 == nullptr) {return false;}

	m_updateColorMapCommand->mergeWith(other2->m_updateColorMapCommand);
	m_updateRangesCommand->mergeWith(other2->m_updateRangesCommand);

	m_apply = other2->m_apply;
	return true;
}
