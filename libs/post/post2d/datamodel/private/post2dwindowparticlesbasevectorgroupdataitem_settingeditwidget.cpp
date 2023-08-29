#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowparticlesbasetopdataitem.h"
#include "../post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasevectorgroupdataitem_settingeditwidget.h"
#include "post2dwindowparticlesbasevectorgroupdataitem_updatesettingcommand.h"
#include "ui_post2dwindowparticlesbasevectorgroupdataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/filter/generalfilteringsettingeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

Post2dWindowParticlesBaseVectorGroupDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowParticlesBaseVectorGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_filteringEditWidget {new GeneralFilteringSettingEditWidget(this)},
	m_item {item},
	ui(new Ui::Post2dWindowParticlesBaseVectorGroupDataItem_SettingEditWidget)
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

Post2dWindowParticlesBaseVectorGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowParticlesBaseVectorGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return new UpdateSettingCommand(apply,
				ui->arrowsSettingWidget->createModifyCommand(),
				m_filteringEditWidget->createModifyCommand(),
				m_item);
}
