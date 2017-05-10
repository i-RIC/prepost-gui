#include "post3dwindowcontourgroupdataitem_setsettingcommand.h"
#include "../post3dwindowgridtypedataitem.h"
#include "../post3dwindowcontourgroupdataitem.h"
#include "../post3dwindowcontourgrouptopdataitem.h"

#include <misc/stringtool.h>

#include <vtkScalarBarActor.h>

Post3dWindowContourGroupDataItem::SetSettingCommand::SetSettingCommand(const ScalarSettingContainer& scalarSetting, const LookupTableContainer& lookupTable, const QString& scalarBarTitle, Post3dWindowContourGroupDataItem* item, QUndoCommand* parent) :
	QUndoCommand(parent),
	m_newScalarSetting {scalarSetting},
	m_newLookupTable {lookupTable},
	m_newScalarBarTitle {scalarBarTitle},
	m_oldScalarSetting {item->scalarSetting()},
	m_oldLookupTable {*(item->lookupTable())},
	m_item {item},
	m_topItem {dynamic_cast<Post3dWindowContourGroupTopDataItem*>(item->parent())}
{
	m_oldScalarBarTitle = m_topItem->m_colorBarTitleMap[item->scalarSetting().target];
}

void Post3dWindowContourGroupDataItem::SetSettingCommand::redo()
{
	applySetting(m_newScalarSetting, m_newLookupTable, m_newScalarBarTitle);
}

void Post3dWindowContourGroupDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldScalarSetting, m_oldLookupTable, m_oldScalarBarTitle);
}

void Post3dWindowContourGroupDataItem::SetSettingCommand::applySetting(const ScalarSettingContainer &scalarSetting, const LookupTableContainer &lookupTable, const QString &scalarBarTitle)
{
	m_item->m_scalarSetting = scalarSetting;
	m_item->m_scalarSetting.scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
	m_item->m_scalarSetting.scalarBarSetting.titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_item->m_scalarSetting.scalarBarSetting.labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

	m_topItem->m_colorBarTitleMap[iRIC::toStr(scalarSetting.target)] = scalarBarTitle;

	Post3dWindowGridTypeDataItem* gtItem = dynamic_cast<Post3dWindowGridTypeDataItem*>(m_item->parent()->parent()->parent());
	LookupTableContainer* lut = gtItem->nodeLookupTable(iRIC::toStr(scalarSetting.target));
	*lut = lookupTable;
}
