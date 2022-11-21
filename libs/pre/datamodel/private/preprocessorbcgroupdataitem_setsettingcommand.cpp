#include "preprocessorbcgroupdataitem_setsettingcommand.h"


PreProcessorBCGroupDataItem::SetSettingCommand::SetSettingCommand(const vtkTextPropertySettingContainer& setting, PreProcessorBCGroupDataItem* item) :
	QUndoCommand(PreProcessorBCGroupDataItem::tr("Edit Display Setting")),
	m_newNameSetting {setting},
	m_oldNameSetting {item->m_nameSetting},
	m_item {item}
{}

void PreProcessorBCGroupDataItem::SetSettingCommand::redo()
{
	apply(m_newNameSetting);
}

void PreProcessorBCGroupDataItem::SetSettingCommand::undo()
{
	apply(m_oldNameSetting);
}

void PreProcessorBCGroupDataItem::SetSettingCommand::apply(const vtkTextPropertySettingContainer& setting)
{
	m_item->m_nameSetting = setting;
	m_item->updateNameActorSettingsOfChildren();
}
