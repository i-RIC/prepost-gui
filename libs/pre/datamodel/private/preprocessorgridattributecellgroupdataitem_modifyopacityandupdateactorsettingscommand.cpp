#include "preprocessorgridattributecellgroupdataitem_modifyopacityandupdateactorsettingscommand.h"

PreProcessorGridAttributeCellGroupDataItem::ModifyOpacityAndUpdateActorSettingsCommand::ModifyOpacityAndUpdateActorSettingsCommand(int opacityPercent, QUndoCommand* child, PreProcessorGridAttributeCellGroupDataItem* item) :
	QUndoCommand {child->text()},
	m_newOpacityPercent {opacityPercent},
	m_oldOpacityPercent {item->m_opacity},
	m_command {child},
	m_item {item}
{}

void PreProcessorGridAttributeCellGroupDataItem::ModifyOpacityAndUpdateActorSettingsCommand::redo()
{
	m_command.get()->redo();
	m_item->m_opacity = m_newOpacityPercent;
	m_item->updateActorSettings();
}

void PreProcessorGridAttributeCellGroupDataItem::ModifyOpacityAndUpdateActorSettingsCommand::undo()
{
	m_command.get()->undo();
	m_item->m_opacity = m_oldOpacityPercent;
	m_item->updateActorSettings();
}
