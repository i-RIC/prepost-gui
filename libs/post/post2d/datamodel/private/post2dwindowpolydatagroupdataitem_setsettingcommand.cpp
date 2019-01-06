#include "post2dwindowpolydatagroupdataitem_setsettingcommand.h"
#include "../post2dwindowgridtypedataitem.h"

#include <misc/stringtool.h>

#include <vtkScalarBarActor.h>
#include <vtkScalarBarWidget.h>

Post2dWindowPolyDataGroupDataItem::SetSettingCommand::SetSettingCommand(const ScalarSettingContainer& setting, const LookupTableContainer& ltc, int lineWidth, const QColor& color, const QString& colorbarTitle, Post2dWindowPolyDataGroupDataItem* item) :
	QUndoCommand(QObject::tr("Update Polygon Scalar Setting")),
	m_newSetting {setting},
	m_newLookupTable {ltc},
	m_newLineWidth {lineWidth},
	m_newColor {color},
	m_newScalarBarTitle {colorbarTitle},
	m_oldSetting {item->m_scalarSetting},
	m_item {item}
{
	if (m_newSetting.target != ""){
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(item->parent()->parent()->parent());
		LookupTableContainer* lut = gtItem->polyDataLookupTable(iRIC::toStr(m_newSetting.target));
		m_oldLookupTable = *lut;
		m_oldScalarBarTitle = item->m_scalarbarTitleMap[iRIC::toStr(m_newSetting.target)];
	}
	m_oldLineWidth = item->m_setting.lineWidth;
	m_oldColor = item->color();
}

void Post2dWindowPolyDataGroupDataItem::SetSettingCommand::redo()
{
	applySetting(m_newSetting, m_newLookupTable, m_newLineWidth, m_newColor, m_newScalarBarTitle);
}

void Post2dWindowPolyDataGroupDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldSetting, m_oldLookupTable, m_oldLineWidth, m_oldColor, m_oldScalarBarTitle);
}

void Post2dWindowPolyDataGroupDataItem::SetSettingCommand::applySetting(const ScalarSettingContainer& setting, const LookupTableContainer& LookupTable, int lineWidth, const QColor& color, const QString& title)
{
	m_item->m_setting.lineWidth = lineWidth;
	m_item->setColor(color);

	m_item->m_scalarSetting = setting;

	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(m_item->parent()->parent()->parent());
	if (m_newSetting.target != ""){
		auto strTarget = iRIC::toStr(m_newSetting.target);
		LookupTableContainer* lut = gtItem->polyDataLookupTable(strTarget);
		*lut = LookupTable;
		lut->update();
		m_item->m_scalarbarTitleMap[strTarget] = title;
	}
	m_item->m_scalarSetting.scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
	m_item->m_scalarSetting.scalarBarSetting.titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_item->m_scalarSetting.scalarBarSetting.labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

	m_item->setTarget(iRIC::toStr(setting.target));
}
