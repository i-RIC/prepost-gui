#include "../post3dwindowparticlesbasetopdataitem.h"
#include "post3dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand::UpdateSettingCommand(bool apply, QUndoCommand* arrowsCommand, QUndoCommand* filterCommand, Post3dWindowParticlesBaseVectorGroupDataItem* item) :
	m_apply {apply},
	m_arrowsCommand {arrowsCommand},
	m_filterCommand {filterCommand},
	m_item {item}
{}

Post3dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand::~UpdateSettingCommand()
{}

void Post3dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand::redo()
{
	m_arrowsCommand->redo();
	m_filterCommand->redo();

	m_item->updateCheckState();
	m_item->topDataItem()->update();
}

void Post3dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand::undo()
{
	m_arrowsCommand->undo();
	m_filterCommand->undo();

	m_item->updateCheckState();
	m_item->topDataItem()->update();
}

int Post3dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand");
}

bool Post3dWindowParticlesBaseVectorGroupDataItem::UpdateSettingCommand::mergeWith(const QUndoCommand* other)
{
	if (! m_apply) {return false;}

	auto other2 = dynamic_cast<const UpdateSettingCommand*> (other);
	if (other2 == nullptr) {return false;}
	if (m_item != other2->m_item) {return false;}

	m_arrowsCommand->mergeWith(other2->m_arrowsCommand.get());
	m_filterCommand->mergeWith(other2->m_filterCommand.get());
	m_apply = other2->m_apply;

	return true;
}
