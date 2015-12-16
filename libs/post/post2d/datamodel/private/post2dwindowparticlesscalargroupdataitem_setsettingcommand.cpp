#include "post2dwindowparticlesscalargroupdataitem_setsettingcommand.h"
#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowparticlestopdataitem.h"

#include <misc/stringtool.h>

#include <vtkScalarBarActor.h>

Post2dWindowParticlesScalarGroupDataItem::SetSettingCommand::SetSettingCommand(const ScalarSettingContainer& setting, const LookupTableContainer& ltc, int size, const QColor& color, const QString& colorbarTitle, Post2dWindowParticlesScalarGroupDataItem* item) :
	QUndoCommand(QObject::tr("Update Particles Scalar Setting")),
	m_newSetting {setting},
	m_newLookupTable {ltc},
	m_newSize {size},
	m_newColor {color},
	m_newScalarBarTitle {colorbarTitle},
	m_oldSetting {item->m_setting},
	m_item {item}
{
	if (m_newSetting.target != ""){
		Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(item->parent()->parent()->parent());
		LookupTableContainer* lut = gtItem->particleLookupTable(iRIC::toStr(m_newSetting.target));
		m_oldLookupTable = *lut;
		m_oldScalarBarTitle = item->m_scalarbarTitleMap.value(iRIC::toStr(m_newSetting.target));
	}
	Post2dWindowParticlesTopDataItem* tItem = dynamic_cast<Post2dWindowParticlesTopDataItem*>(item->parent());
	m_oldSize = tItem->size();
	m_oldColor = tItem->color();
}

void Post2dWindowParticlesScalarGroupDataItem::SetSettingCommand::redo()
{
	applySetting(m_newSetting, m_newLookupTable, m_newSize, m_newColor, m_newScalarBarTitle);
}

void Post2dWindowParticlesScalarGroupDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldSetting, m_oldLookupTable, m_oldSize, m_oldColor, m_oldScalarBarTitle);
}

void Post2dWindowParticlesScalarGroupDataItem::SetSettingCommand::applySetting(const ScalarSettingContainer& setting, const LookupTableContainer& LookupTable, int size, const QColor& color, const QString& title)
{
	Post2dWindowParticlesTopDataItem* tItem = dynamic_cast<Post2dWindowParticlesTopDataItem*>(m_item->parent());
	tItem->setSize(size);
	tItem->setColor(color);

	m_item->m_setting = setting;

	Post2dWindowGridTypeDataItem* gtItem = dynamic_cast<Post2dWindowGridTypeDataItem*>(m_item->parent()->parent()->parent());
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
