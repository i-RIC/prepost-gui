#include "measureddatapointgroupdataitem_impl.h"
#include "measureddatapointgroupdataitem_updatesettingcommand.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

MeasuredDataPointGroupDataItem::UpdateSettingCommand::UpdateSettingCommand(const MeasuredDataPointSetting& s, ColorMapSettingContainer* cs, MeasuredDataPointGroupDataItem* item) :
	QUndoCommand {},
	m_settingCommand {new ValueModifyCommmand<MeasuredDataPointSetting>("Modify Setting", s, &item->impl->m_setting)},
	m_colormapCommand {nullptr},
	m_item {item}
{
	if (cs != nullptr) {
		auto oldcs = item->colorMapSetting(iRIC::toStr(s.value));
		m_colormapCommand = new ValueModifyCommmand<ColorMapSettingContainer> ("Modify Colormap", *cs, oldcs);
	}
}

void MeasuredDataPointGroupDataItem::UpdateSettingCommand::redo()
{
	m_settingCommand->redo();
	if (m_colormapCommand != nullptr) {
		m_colormapCommand->redo();
	}

	m_item->impl->updateCheckState();
	m_item->impl->updateActorSettings();
}

void MeasuredDataPointGroupDataItem::UpdateSettingCommand::undo()
{
	m_settingCommand->undo();
	if (m_colormapCommand != nullptr) {
		m_colormapCommand->undo();
	}

	m_item->impl->updateCheckState();
	m_item->impl->updateActorSettings();
}
