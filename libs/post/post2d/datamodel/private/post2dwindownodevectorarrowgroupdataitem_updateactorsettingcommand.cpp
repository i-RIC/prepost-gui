#include "post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h"

#include <misc/qundocommandhelper.h>

Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::UpdateActorSettingCommand(bool apply, QUndoCommand* command, Post2dWindowNodeVectorArrowGroupDataItem* item) :
	m_apply {apply},
	m_command {command},
	m_item {item}
{}

void Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::redo()
{
	m_command->redo();
	m_item->updateActorSetting();
}

void Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::undo()
{
	m_command->undo();
	m_item->updateActorSetting();
}

int Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::id() const
{
	return iRIC::generateCommandId("Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingsCommand");
}

bool Post2dWindowNodeVectorArrowGroupDataItem::UpdateActorSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto c = dynamic_cast<const UpdateActorSettingCommand*> (other);
	if (c == nullptr) {return false;}

	return m_command->mergeWith(c->m_command.get());
}
