#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget_additionalwidgets.h"
#include "post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h"
#include "ui_post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/filter/structured2dfilteringsettingeditwidget.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/region/region2dsettingeditwidget.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowNodeVectorArrowGroupStructuredDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	m_additionalWidgets {new AdditionalWidgets(this)},
	ui(new Ui::Post2dWindowNodeVectorArrowGroupStructuredDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	ui->arrowsSettingWidget->setAdditionalSettingWidget(m_additionalWidgets);

	auto dataContainer = item->zoneDataItem()->dataContainer();
	auto data = dataContainer->data()->data();
	if (! dataContainer->IBCExists()) {
		m_additionalWidgets->regionWidget()->disableActive();
	}
	auto structured = vtkStructuredGrid::SafeDownCast(data);
	int dims[3];
	structured->GetDimensions(dims);
	m_additionalWidgets->regionWidget()->setDimensions(dims[0], dims[1]);

	std::map<std::string, QString> names;
	auto pd = data->GetPointData();
	auto tItem = item->zoneDataItem()->gridTypeDataItem();
	auto gridType = tItem->gridType();
	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd)) {
		names.insert({name, gridType->solutionCaption(name)});
	}
	ui->arrowsSettingWidget->setGridType(gridType);
	ui->arrowsSettingWidget->setValueNames(names);
	ui->arrowsSettingWidget->setColorMapSettings(m_item->m_colorMapSettings);
	ui->arrowsSettingWidget->setSetting(&item->m_setting);
	m_additionalWidgets->samplingWidget()->setSetting(&item->m_filteringSetting);
	m_additionalWidgets->regionWidget()->setSetting(&item->m_regionSetting);
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget"), apply);
	command->addCommand(ui->arrowsSettingWidget->createModifyCommand());
	command->addCommand(m_additionalWidgets->samplingWidget()->createModifyCommand());
	command->addCommand(m_additionalWidgets->regionWidget()->createModifyCommand());

	return new UpdateActorSettingCommand(apply, command, m_item);
}
