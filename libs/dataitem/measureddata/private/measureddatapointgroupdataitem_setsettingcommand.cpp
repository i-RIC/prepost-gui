#include "measureddatapointgroupdataitem_impl.h"
#include "measureddatapointgroupdataitem_setsettingcommand.h"

#include <misc/stringtool.h>

#include <vtkScalarBarWidget.h>

MeasuredDataPointGroupDataItem::SetSettingCommand::SetSettingCommand(const MeasuredDataPointSetting& setting, const LookupTableContainer& lut, const QString& title, MeasuredDataPointGroupDataItem* item) :
	m_newSetting {setting},
	m_newLookupTable {lut},
	m_newScalarBarTitle {title},
	m_oldSetting {item->impl->m_setting},
	m_oldScalarBarTitle {item->impl->m_colorbarTitleMap[iRIC::toStr(setting.target)]},
	m_item {item}
{
	auto oldlut = item->lookupTable(iRIC::toStr(setting.target));
	if (oldlut != nullptr) {
		m_oldLookupTable = *oldlut;
	}
}

void MeasuredDataPointGroupDataItem::SetSettingCommand::redo()
{
	applySetting(m_newSetting, m_newLookupTable, m_newScalarBarTitle);
}

void MeasuredDataPointGroupDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldSetting, m_oldLookupTable, m_oldScalarBarTitle);
}

void MeasuredDataPointGroupDataItem::SetSettingCommand::applySetting(const MeasuredDataPointSetting& setting, LookupTableContainer& lut, const QString& title)
{
	m_item->impl->m_setting = setting;
	auto target = iRIC::toStr(setting.target);
	m_item->setTarget(target);
	auto lutp = m_item->lookupTable(target);
	*lutp = lut;
	lutp->update();
	setting.scalarBarSetting.saveToRepresentation(m_item->impl->m_scalarBarWidget->GetScalarBarRepresentation());
	m_item->impl->m_colorbarTitleMap[target] = title;

	m_item->impl->updateActorSettings();
}
