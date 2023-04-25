#include "post3dwindowcontourgroupdataitem_updateactorsettingcommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowContourGroupDataItem::UpdateActorSettingCommand::UpdateActorSettingCommand(bool apply, QUndoCommand* updateColorMapCommand, QUndoCommand* updateFacesCommand, Post3dWindowContourGroupDataItem* item) :
	QUndoCommand {},
	m_apply {apply},
	m_updateColorMapCommand {updateColorMapCommand},
	m_updateFacesCommand {updateFacesCommand},
	m_item {item}
{}

void Post3dWindowContourGroupDataItem::UpdateActorSettingCommand::redo()
{
	m_updateColorMapCommand->redo();
	m_updateFacesCommand->redo();

	m_item->updateActorSetting();
}

void Post3dWindowContourGroupDataItem::UpdateActorSettingCommand::undo()
{
	m_updateColorMapCommand->undo();
	m_updateFacesCommand->undo();

	m_item->updateActorSetting();
}

int Post3dWindowContourGroupDataItem::UpdateActorSettingCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowContourGroupDataItem::UpdateActorSettingsCommand");
}

bool Post3dWindowContourGroupDataItem::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (other2 == nullptr) {return false;}

	m_updateColorMapCommand->mergeWith(other2->m_updateColorMapCommand);
	m_updateFacesCommand->mergeWith(other2->m_updateFacesCommand);

	m_apply = other2->m_apply;
	return true;
}
