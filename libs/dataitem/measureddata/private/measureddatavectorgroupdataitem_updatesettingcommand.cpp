#include "measureddatavectorgroupdataitem_impl.h"
#include "measureddatavectorgroupdataitem_updatesettingcommand.h"

MeasuredDataVectorGroupDataItem::UpdateSettingCommand::UpdateSettingCommand(QUndoCommand* arrowsCommand, QUndoCommand* filterCommand, MeasuredDataVectorGroupDataItem* item) :
	m_arrowsCommand {arrowsCommand},
	m_filterCommand {filterCommand},
	m_item {item}
{}

void MeasuredDataVectorGroupDataItem::UpdateSettingCommand::redo()
{
	m_arrowsCommand->redo();
	m_filterCommand->redo();

	m_item->impl->updateCheckState();
	m_item->impl->updateActorSettings();
}

void MeasuredDataVectorGroupDataItem::UpdateSettingCommand::undo()
{
	m_arrowsCommand->undo();
	m_filterCommand->undo();

	m_item->impl->updateCheckState();
	m_item->impl->updateActorSettings();
}
