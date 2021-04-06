#include "post3dwindowcellcontourgroupdataitem_setsettingcommand.h"
#include "../post3dwindowcellcontourdataitem.h"
#include "../post3dwindowcellcontourgrouptopdataitem.h"
#include "../post3dwindowgridtypedataitem.h"

#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <misc/stringtool.h>

Post3dWindowCellContourGroupDataItem::SetSettingCommand::SetSettingCommand(const ScalarSettingContainer& scalarSetting, const std::vector<Post3dCellRangeSettingContainer>& rangeSettings, const LookupTableContainer& lookupTable, QString& scalarBarTitle, Post3dWindowCellContourGroupDataItem* item) :
	m_newScalarSetting {scalarSetting},
	m_newRangeSettings (rangeSettings),
	m_newLookupTable {lookupTable},
	m_newScalarBarTitle {scalarBarTitle},
	m_item {item}
{
	m_oldScalarSetting = m_item->m_setting;
	for (auto c : item->m_childItems) {
		auto c2 = dynamic_cast<Post3dWindowCellContourDataItem*> (c);
		m_oldRangeSettings.push_back(c2->setting());
	}
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*> (m_item->parent()->parent()->parent());
	m_oldLookupTable = *(gtItem->cellLookupTable(iRIC::toStr(scalarSetting.target)));

	auto tItem = dynamic_cast<Post3dWindowCellContourGroupTopDataItem*> (m_item->parent());
	m_oldScalarBarTitle = tItem->m_colorBarTitleMap[scalarSetting.target];
}

Post3dWindowCellContourGroupDataItem::SetSettingCommand::~SetSettingCommand()
{}

void Post3dWindowCellContourGroupDataItem::SetSettingCommand::redo()
{
	applySetting(m_newScalarSetting, m_newRangeSettings, m_newLookupTable, m_newScalarBarTitle);
}

void Post3dWindowCellContourGroupDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldScalarSetting, m_oldRangeSettings, m_oldLookupTable, m_oldScalarBarTitle);
}

void Post3dWindowCellContourGroupDataItem::SetSettingCommand::applySetting(const ScalarSettingContainer& scalarSetting, const std::vector<Post3dCellRangeSettingContainer> rangeSettings, const LookupTableContainer& lookupTable, const QString& scalarBarTitle)
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*> (m_item->parent()->parent()->parent());
	auto lut = gtItem->cellLookupTable(iRIC::toStr(scalarSetting.target));
	*lut = lookupTable;

	auto tItem = dynamic_cast<Post3dWindowCellContourGroupTopDataItem*> (m_item->parent());
	tItem->m_colorBarTitleMap[m_newScalarSetting.target] = scalarBarTitle;

	m_item->setSetting(scalarSetting, rangeSettings);
}
