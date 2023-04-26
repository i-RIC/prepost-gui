#include "preprocessorgridattributenodegroupdataitem_modifyopacityandupdateactorsettingscommand.h"

PreProcessorGridAttributeNodeGroupDataItem::ModifyOpacityAndUpdateActorSettingsCommand::ModifyOpacityAndUpdateActorSettingsCommand(int opacityPercent, QUndoCommand* child, PreProcessorGridAttributeNodeGroupDataItem* item) :
	QUndoCommand {child->text()},
	m_newOpacityPercent {opacityPercent},
	m_oldOpacityPercent {item->m_opacity},
	m_command {child},
	m_item {item}
{}

void PreProcessorGridAttributeNodeGroupDataItem::ModifyOpacityAndUpdateActorSettingsCommand::redo()
{
	m_command.get()->redo();
	m_item->m_opacity = m_newOpacityPercent;
	m_item->updateActorSetting();
}

void PreProcessorGridAttributeNodeGroupDataItem::ModifyOpacityAndUpdateActorSettingsCommand::undo()
{
	m_command.get()->undo();
	m_item->m_opacity = m_oldOpacityPercent;
	m_item->updateActorSetting();
}
