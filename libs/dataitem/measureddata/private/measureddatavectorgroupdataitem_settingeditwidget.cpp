#include "../measureddatafiledataitem.h"
#include "../measureddatapointgroupdataitem.h"
#include "../measureddatavectorgrouptopdataitem.h"
#include "measureddatavectorgroupdataitem_impl.h"
#include "measureddatavectorgroupdataitem_settingeditwidget.h"
#include "ui_measureddatavectorgroupdataitem_settingeditwidget.h"

#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/filter/generalfilteringsettingeditwidget.h>
#include <guicore/project/measured/measureddata.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

MeasuredDataVectorGroupDataItem::SettingEditWidget::SettingEditWidget(MeasuredDataVectorGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_filteringEditWidget {new GeneralFilteringSettingEditWidget(this)},
	m_item {item},
	ui(new Ui::MeasuredDataVectorGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	ui->arrowsSettingWidget->setAdditionalSettingWidget(m_filteringEditWidget);

	auto fileDataItem = item->topDataItem()->fileDataItem();
	auto gType = item->projectData()->solverDefinition()->dummyGridType();

	ui->arrowsSettingWidget->setGridType(gType);
	ui->arrowsSettingWidget->setColorMapSettings(fileDataItem->pointGroupDataItem()->colorMapSettings());

	ui->arrowsSettingWidget->setSetting(&m_item->impl->m_setting.arrowsSetting);
	m_filteringEditWidget->setSetting(&m_item->impl->m_setting.filteringSetting);
}

MeasuredDataVectorGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* MeasuredDataVectorGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("MeasuredDataVectorGroupDataItem::SettingEditWidget"), apply);
	command->addCommand(ui->arrowsSettingWidget->createModifyCommand());
	command->addCommand(m_filteringEditWidget->createModifyCommand());

	return command;
}
