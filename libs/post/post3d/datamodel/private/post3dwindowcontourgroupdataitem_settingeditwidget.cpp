#include "post3dwindowcontourgroupdataitem_impl.h"
#include "post3dwindowcontourgroupdataitem_setfacescommand.h"
#include "post3dwindowcontourgroupdataitem_settingeditwidget.h"
#include "ui_post3dwindowcontourgroupdataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <vtkStructuredGrid.h>

Post3dWindowContourGroupDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowContourGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_colorMapWidget {new ColorMapSettingEditWidget {this}},
	m_item {item},
	ui(new Ui::Post3dWindowContourGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto w = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapWidget, this);
	ui->colorMapWidget->setWidget(w);

	m_colorMapWidget->setSetting(&item->impl->m_colorMapSetting);
	setSetting(item->impl->m_setting);

	auto grid = vtkStructuredGrid::SafeDownCast(m_item->data()->data()->data());
	ui->faceListWidget->setDimensions(grid->GetDimensions());
	ui->faceListWidget->setFaces(item->faces());
}

Post3dWindowContourGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowContourGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post3dWindowContourGroupDataItem::SettingEditWidget"), apply);

	command->addCommand(m_colorMapWidget->createModifyCommand());
	command->addCommand(new ValueModifyCommmand<Setting> (iRIC::generateCommandId("Setting"), apply, setting(), &m_item->impl->m_setting));
	command->addCommand(new SetFacesCommand(ui->faceListWidget->faces(), m_item));

	return command;
}

Post3dWindowContourGroupDataItem::Setting Post3dWindowContourGroupDataItem::SettingEditWidget::setting() const
{
	Setting setting;

	setting.contourSetting = ui->contourWidget->setting();
	setting.lighting = ui->lightingCheckBox->isChecked();
	setting.opacity = ui->opacityWidget->opacityPercent();

	return setting;
}

void Post3dWindowContourGroupDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->contourWidget->setSetting(setting.contourSetting);
	ui->lightingCheckBox->setChecked(setting.lighting);
	ui->opacityWidget->setOpacityPercent(setting.opacity);
}
