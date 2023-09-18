#include "post3dwindowcontourgroupdataitem_impl.h"
#include "post3dwindowcontourgroupdataitem_setfacescommand.h"
#include "post3dwindowcontourgroupdataitem_settingeditwidget.h"
#include "../post3dwindowgridtypedataitem.h"
#include "../post3dwindowzonedataitem.h"
#include "ui_post3dwindowcontourgroupdataitem_settingeditwidget.h"

#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <vtkStructuredGrid.h>

Post3dWindowContourGroupDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowContourGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::Post3dWindowContourGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto output = item->zoneDataItem()->gridTypeDataItem()->gridType()->output(item->target());

	auto cmw = output->createColorMapSettingEditWidget(this);
	cmw->setSetting(item->impl->m_setting.colorMapSetting);
	m_colorMapWidget = new ColorMapSettingEditWidgetWithImportExportButton(cmw, this);

	ui->colorMapWidget->setWidget(m_colorMapWidget);
	setSetting(item->impl->m_setting);

	auto grid = dynamic_cast<v4Structured3dGrid*> (m_item->data()->gridData()->grid());
	ui->faceListWidget->setDimensions(grid->vtkConcreteData()->concreteData()->GetDimensions());
	ui->faceListWidget->setFaces(item->faces());
}

Post3dWindowContourGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowContourGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post3dWindowContourGroupDataItem::SettingEditWidget"), apply);

	command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	command->addCommand(new ValueModifyCommmand<Setting> (iRIC::generateCommandId("Setting"), apply, setting(), &m_item->impl->m_setting));
	command->addCommand(new SetFacesCommand(ui->faceListWidget->faces(), m_item));

	return command;
}

Post3dWindowContourGroupDataItem::Setting Post3dWindowContourGroupDataItem::SettingEditWidget::setting() const
{
	Setting setting;

	setting.contourSetting = ui->contourWidget->setting();
	setting.lighting = ui->lightingCheckBox->isChecked();
	setting.opacity = ui->opacityWidget->opacity();

	return setting;
}

void Post3dWindowContourGroupDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->contourWidget->setSetting(setting.contourSetting);
	ui->lightingCheckBox->setChecked(setting.lighting);
	ui->opacityWidget->setOpacity(setting.opacity);
}
