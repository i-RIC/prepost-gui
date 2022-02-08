#include "../post2dbirdeyewindowgridtypedataitem.h"
#include "../post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "post2dbirdeyewindownodescalargroupdataitem_setsettingcommand.h"

#include <misc/stringtool.h>

#include <vtkScalarBarActor.h>

Post2dBirdEyeWindowNodeScalarGroupDataItem::SetSettingCommand::SetSettingCommand(const Post2dBirdEyeWindowContourSetting& s, const LookupTableContainer& ltc, const QString& colorbarTitle, Post2dBirdEyeWindowNodeScalarGroupDataItem* item, QUndoCommand* parent) :
	QUndoCommand {Post2dBirdEyeWindowNodeScalarGroupDataItem::tr("Update Contour Setting"), parent},
	m_newSetting {s},
	m_newLookupTable {ltc},
	m_newScalarBarTitle {colorbarTitle},
	m_oldSetting {item->m_setting},
	m_oldLookupTable {},
	m_item {item},
	m_topItem {dynamic_cast<Post2dBirdEyeWindowNodeScalarGroupTopDataItem*>(item->parent())}
{
	m_oldScalarBarTitle = m_topItem->m_colorbarTitleMap[item->target()];

	Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(item->parent()->parent()->parent());
	if (LookupTableContainer* lut = gtItem->nodeLookupTable(item->target())) {
		m_oldLookupTable = *lut;
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SetSettingCommand::redo()
{
	m_item->m_setting = m_newSetting;
	applySettings(m_newSetting.target, m_newLookupTable, m_newScalarBarTitle);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SetSettingCommand::undo()
{
	m_item->m_setting = m_oldSetting;
	applySettings(m_oldSetting.target, m_oldLookupTable, m_oldScalarBarTitle);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SetSettingCommand::applySettings(const QString& t, const LookupTableContainer& c, const QString& title)
{
	std::string target = iRIC::toStr(t);
	Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(m_item->parent()->parent()->parent());
	if (LookupTableContainer* lut = gtItem->nodeLookupTable(target)) {
		*lut = c;
		lut->update();
	}

	m_topItem->m_colorbarTitleMap[target] = title;

	m_item->m_setting.scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
	m_item->m_setting.scalarBarSetting.titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_item->m_setting.scalarBarSetting.labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
	m_item->setTarget(target);
}
