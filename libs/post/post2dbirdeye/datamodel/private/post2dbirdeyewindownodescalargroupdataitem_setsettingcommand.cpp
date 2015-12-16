#include "../post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindownodescalargroupdataitem_setsettingcommand.h"

#include <misc/stringtool.h>

Post2dBirdEyeWindowNodeScalarGroupDataItem::SetSettingCommand::SetSettingCommand(const Post2dWindowContourSetting& s, const LookupTableContainer& ltc, const QString& colorbarTitle, Post2dBirdEyeWindowNodeScalarGroupDataItem* item) :
	QUndoCommand {Post2dBirdEyeWindowNodeScalarGroupDataItem::tr("Update Contour Setting")},
	m_newSetting {s},
	m_newLookupTable {ltc},
	m_newScalarBarTitle {colorbarTitle},
	m_oldSetting {item->m_setting},
	m_oldLookupTable {},
	m_oldScalarBarTitle {item->m_colorbarTitleMap[iRIC::toStr(s.target)]},
	m_item {item}
{
	Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(item->parent()->parent());
	LookupTableContainer* lut = gtItem->nodeLookupTable(s.target);
	m_oldLookupTable = *lut;
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
	Post2dBirdEyeWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dBirdEyeWindowGridTypeDataItem*>(m_item->parent()->parent());
	LookupTableContainer* lut = gtItem->nodeLookupTable(target);
	*lut = c;
	lut->update();

	m_item->m_colorbarTitleMap[target] = title;

	m_item->m_setting.scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
	m_item->m_setting.scalarBarSetting.titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_item->m_setting.scalarBarSetting.labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());
	m_item->setTarget(target);
}
