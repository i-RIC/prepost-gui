#include "post3dwindowcellcontourgrouptopdataitem_createcommand.h"
#include "../post3dwindowcellcontourgroupdataitem.h"
#include "../post3dwindowgridtypedataitem.h"

#include <misc/stringtool.h>

#include <QStandardItem>

Post3dWindowCellContourGroupTopDataItem::CreateCommand::CreateCommand(const ScalarSettingContainer& scalarSetting, const std::vector<Post3dCellRangeSettingContainer>& rangeSettings, const LookupTableContainer& lookupTable, QString& scalarBarTitle, Post3dWindowCellContourGroupTopDataItem* item) :
	m_scalarSetting {scalarSetting},
	m_rangeSettings (rangeSettings),
	m_newLookupTable {lookupTable},
	m_newScalarBarTitle {scalarBarTitle},
	m_newItem {nullptr},
	m_item {item}
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*> (m_item->parent()->parent());
	m_oldLookupTable = *(gtItem->cellLookupTable(iRIC::toStr(m_scalarSetting.target)));
	m_oldScalarBarTitle = m_item->m_colorBarTitleMap[m_scalarSetting.target];
}

Post3dWindowCellContourGroupTopDataItem::CreateCommand::~CreateCommand()
{}

void Post3dWindowCellContourGroupTopDataItem::CreateCommand::redo()
{
	m_newItem = new Post3dWindowCellContourGroupDataItem(m_item);
	m_newItem->setIsCommandExecuting(true);

	m_newItem->standardItem()->setText(m_scalarSetting.target);
	m_newItem->updateZScale(m_item->m_zScale);
	m_newItem->setSetting(m_scalarSetting, m_rangeSettings);
	m_item->m_childItems.push_back(m_newItem);

	m_newItem->setIsCommandExecuting(false);
	m_item->updateItemMap();

	updateScalarBarData(m_newLookupTable, m_newScalarBarTitle);
}

void Post3dWindowCellContourGroupTopDataItem::CreateCommand::undo()
{
	delete m_newItem;
	m_newItem = nullptr;

	m_item->update();
	m_item->updateItemMap();

	updateScalarBarData(m_oldLookupTable, m_oldScalarBarTitle);
}

void Post3dWindowCellContourGroupTopDataItem::CreateCommand::updateScalarBarData(const LookupTableContainer& lookupTable, const QString& scalarBarTitle)
{
	auto gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*> (m_item->parent()->parent());
	auto lut = gtItem->cellLookupTable(iRIC::toStr(m_scalarSetting.target));
	*lut = lookupTable;

	m_item->m_colorBarTitleMap[m_scalarSetting.target] = scalarBarTitle;
}
