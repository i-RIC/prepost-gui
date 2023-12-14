#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindownodevectorarrowgrouptopdataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget.h"
#include "post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget_additionalwidgets.h"
#include "post2dwindownodevectorarrowgroupdataitem_updateactorsettingcommand.h"
#include "ui_post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/filter/structured2dfilteringsettingeditwidget.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
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

	auto cont = item->topDataItem()->zoneDataItem()->v4DataContainer();
	auto data = cont->gridData()->grid();
	if (! cont->gridData()->ibcExists(v4SolutionGrid::Position::Node)) {
		m_additionalWidgets->regionWidget()->disableActive();
	}
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (data);
	int dims[3];
	sGrid->vtkConcreteData()->concreteData()->GetDimensions(dims);
	m_additionalWidgets->regionWidget()->setDimensions(dims[0], dims[1]);

	auto tItem = item->topDataItem()->zoneDataItem()->gridTypeDataItem();
	auto gridType = tItem->gridType();
	ui->arrowsSettingWidget->setGridType(gridType);
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