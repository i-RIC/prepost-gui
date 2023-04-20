#include "post3dwindowcellcontourgroupdataitem_updateactorsettingcommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowCellContourGroupDataItem::UpdateActorSettingCommand::UpdateActorSettingCommand(bool apply, QUndoCommand* updateColorMapCommand, QUndoCommand* updateRangesCommand, Post3dWindowCellContourGroupDataItem* item) :
	QUndoCommand {},
	m_apply {apply},
	m_updateColorMapCommand {updateColorMapCommand},
	m_updateRangesCommand {updateRangesCommand},
	m_item {item}
{}

void Post3dWindowCellContourGroupDataItem::UpdateActorSettingCommand::redo()
{
	m_updateColorMapCommand->redo();
	m_updateRangesCommand->redo();

	m_item->updateActorSetting();
}

void Post3dWindowCellContourGroupDataItem::UpdateActorSettingCommand::undo()
{
	m_updateColorMapCommand->undo();
	m_updateRangesCommand->undo();

	m_item->updateActorSetting();
}

int Post3dWindowCellContourGroupDataItem::UpdateActorSettingCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowCellContourGroupDataItem::UpdateActorSettingsCommand");
}

bool Post3dWindowCellContourGroupDataItem::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (other2 == nullptr) {return false;}

	m_updateColorMapCommand->mergeWith(other2->m_updateColorMapCommand);
	m_updateRangesCommand->mergeWith(other2->m_updateRangesCommand);

	m_apply = other2->m_apply;
	return true;
}
