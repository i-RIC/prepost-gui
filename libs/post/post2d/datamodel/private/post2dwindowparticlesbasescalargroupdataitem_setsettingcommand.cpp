#include "post2dwindowparticlesbasescalargroupdataitem_setsettingcommand.h"
#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowparticlesbasetopdataitem.h"
#include "../post2dwindowzonedataitem.h"

#include <misc/stringtool.h>

#include <vtkScalarBarActor.h>

Post2dWindowParticlesBaseScalarGroupDataItem::SetSettingCommand::SetSettingCommand(const ScalarSettingContainer& setting, const LookupTableContainer& ltc, int size, const QColor& color, const QString& colorbarTitle, Post2dWindowParticlesBaseScalarGroupDataItem* item) :
	QUndoCommand(QObject::tr("Update Particles Scalar Setting")),
	m_newSetting {setting},
	m_newLookupTable {ltc},
	m_newSize {size},
	m_newColor {color},
	m_newScalarBarTitle {colorbarTitle},
	m_oldSetting {item->m_setting},
	m_item {item}
{
	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (item->parent());
	if (m_newSetting.target != ""){
		auto gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(topItem->zoneDataItem()->parent());
		LookupTableContainer* lut = gtItem->particleLookupTable(iRIC::toStr(m_newSetting.target));
		m_oldLookupTable = *lut;
		m_oldScalarBarTitle = item->m_scalarbarTitleMap.value(iRIC::toStr(m_newSetting.target));
	}
	m_oldSize = topItem->size();
	m_oldColor = topItem->color();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::SetSettingCommand::redo()
{
	applySetting(m_newSetting, m_newLookupTable, m_newSize, m_newColor, m_newScalarBarTitle);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldSetting, m_oldLookupTable, m_oldSize, m_oldColor, m_oldScalarBarTitle);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::SetSettingCommand::applySetting(const ScalarSettingContainer& setting, const LookupTableContainer& LookupTable, int size, const QColor& color, const QString& title)
{
	auto tItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*>(m_item->parent());
	tItem->setSize(size);
	tItem->setColor(color);

	m_item->m_setting = setting;

	auto topItem = dynamic_cast<Post2dWindowParticlesBaseTopDataItem*> (m_item->parent());
	auto gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(topItem->zoneDataItem()->parent());
	if (m_newSetting.target != ""){
		auto strTarget = iRIC::toStr(m_newSetting.target);
		LookupTableContainer* lut = gtItem->particleLookupTable(strTarget);
		*lut = LookupTable;
		lut->update();
		m_item->m_scalarbarTitleMap[strTarget] = title;
	}
	m_item->m_setting.scalarBarSetting.saveToRepresentation(m_item->m_scalarBarWidget->GetScalarBarRepresentation());
	m_item->m_setting.scalarBarSetting.titleTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetTitleTextProperty());
	m_item->m_setting.scalarBarSetting.labelTextSetting.applySetting(m_item->m_scalarBarWidget->GetScalarBarActor()->GetLabelTextProperty());

	m_item->setTarget(iRIC::toStr(setting.target));
}
