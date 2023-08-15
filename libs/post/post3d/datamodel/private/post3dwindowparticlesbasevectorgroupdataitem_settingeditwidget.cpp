#include "../post3dwindowgridtypedataitem.h"
#include "../post3dwindowparticlesbasetopdataitem.h"
#include "../post3dwindowparticlesbasescalargroupdataitem.h"
#include "post3dwindowparticlesbasevectorgroupdataitem_settingeditwidget.h"
#include "post3dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.h"
#include "ui_post3dwindowparticlesbasevectorgroupdataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/filter/generalfilteringsettingeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

Post3dWindowParticlesBaseVectorGroupDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowParticlesBaseVectorGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_filteringEditWidget {new GeneralFilteringSettingEditWidget(this)},
	m_item {item},
	ui(new Ui::Post3dWindowParticlesBaseVectorGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	ui->arrowsSettingWidget->setAdditionalSettingWidget(m_filteringEditWidget);

	auto gridTypeDataItem = item->gridTypeDataItem();

	auto gtype = gridTypeDataItem->gridType();
	auto colorMaps = m_item->topDataItem()->scalarGroupDataItem()->colorMapSettings();
	ui->arrowsSettingWidget->setGridType(gtype);
	ui->arrowsSettingWidget->setColorMapSettings(colorMaps);
	ui->arrowsSettingWidget->setSetting(&m_item->m_setting.arrowsSetting);
	m_filteringEditWidget->setSetting(&m_item->m_setting.filteringSetting);
}

Post3dWindowParticlesBaseVectorGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowParticlesBaseVectorGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return new UpdateSettingCommand(apply,
				ui->arrowsSettingWidget->createModifyCommand(),
				m_filteringEditWidget->createModifyCommand(),
				m_item);
}
