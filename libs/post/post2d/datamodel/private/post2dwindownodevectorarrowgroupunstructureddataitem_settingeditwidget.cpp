#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindownodevectorarrowgrouptopdataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem_settingeditwidget.h"
#include "post2dwindownodevectorarrowgroupunstructureddataitem_settingeditwidget_additionalwidgets.h"
#include "ui_post2dwindownodevectorarrowgroupunstructureddataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/filter/generalfilteringsettingeditwidget.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/region/region2dsettingeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	m_additionalWidgets {new AdditionalWidgets(this)},
	ui(new Ui::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	ui->arrowsSettingWidget->setAdditionalSettingWidget(m_additionalWidgets);

	auto dataContainer = item->topDataItem()->zoneDataItem()->dataContainer();
	if (! dataContainer->IBCExists()) {
		m_additionalWidgets->regionWidget()->disableActive();
	}
	m_additionalWidgets->regionWidget()->hideCustom();

	auto tItem = item->topDataItem()->zoneDataItem()->gridTypeDataItem();
	auto gridType = tItem->gridType();
	ui->arrowsSettingWidget->setGridType(gridType);
	ui->arrowsSettingWidget->setColorMapSettings(m_item->m_colorMapSettings);

	ui->arrowsSettingWidget->setSetting(&item->m_setting);
	m_additionalWidgets->samplingWidget()->setSetting(&item->m_filteringSetting);
	m_additionalWidgets->regionWidget()->setSetting(&item->m_regionSetting);
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Property"), apply);
	command->addCommand(ui->arrowsSettingWidget->createModifyCommand());
	command->addCommand(m_additionalWidgets->samplingWidget()->createModifyCommand());
	command->addCommand(m_additionalWidgets->regionWidget()->createModifyCommand());

	return new UpdateActorSettingCommand(apply, command, m_item);
}
