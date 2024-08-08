#include "preprocessorgeodatagroupdataitem_applyscalarstocolorssettingcommand.h"

#include <misc/qundocommandhelper.h>

PreProcessorGeoDataGroupDataItem::ApplyScalarsToColorsSettingCommand::ApplyScalarsToColorsSettingCommand(PreProcessorGeoDataGroupDataItem* item) :
	QUndoCommand(),
	m_item {item}
{}

void PreProcessorGeoDataGroupDataItem::ApplyScalarsToColorsSettingCommand::redo()
{
	m_item->applyScalarsToColorsSetting();
}

void PreProcessorGeoDataGroupDataItem::ApplyScalarsToColorsSettingCommand::undo()
{
	m_item->applyScalarsToColorsSetting();
}

int PreProcessorGeoDataGroupDataItem::ApplyScalarsToColorsSettingCommand::id() const
{
	return iRIC::generateCommandId("PreProcessorGeoDataGroupDataItem::ApplyScalarsToColorsSettingCommand");
}

bool PreProcessorGeoDataGroupDataItem::ApplyScalarsToColorsSettingCommand::mergeWith(const QUndoCommand* other)
{
	auto command = dynamic_cast<const ApplyScalarsToColorsSettingCommand*> (other);
	if (command == nullptr) {return false;}

	return true;
}
