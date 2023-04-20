#include "post3dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::UpdateActorSettingCommand(bool apply, QUndoCommand* updateArrowsCommand, QUndoCommand* updateSettingsCommand, QUndoCommand* updateColormapsCommand, Post3dWindowNodeVectorArrowGroupDataItem* item) :
	QUndoCommand {},
	m_apply {apply},
	m_updateArrowsCommand {updateArrowsCommand},
	m_updateSettingsCommand {updateSettingsCommand},
	m_updateColormapsCommand {updateColormapsCommand},
	m_item {item}
{}

void Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::redo()
{
	m_updateArrowsCommand->redo();
	m_updateSettingsCommand->redo();
	m_updateColormapsCommand->redo();

	m_item->updateActorSetting();
}

void Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::undo()
{
	m_updateArrowsCommand->undo();
	m_updateSettingsCommand->undo();
	m_updateColormapsCommand->undo();

	m_item->updateActorSetting();
}

int Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand");
}

bool Post3dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (other2 == nullptr) {return false;}

	m_updateArrowsCommand->mergeWith(other2->m_updateArrowsCommand);
	m_updateSettingsCommand->mergeWith(other2->m_updateSettingsCommand);
	m_updateColormapsCommand->mergeWith(other2->m_updateColormapsCommand);

	m_apply = other2->m_apply;
	return true;
}
