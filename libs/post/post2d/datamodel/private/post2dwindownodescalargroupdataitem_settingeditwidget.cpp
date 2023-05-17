#include "../post2dwindownodescalargrouptopdataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodescalargroupdataitem_impl.h"
#include "post2dwindownodescalargroupdataitem_settingeditwidget.h"
#include "ui_post2dwindownodescalargroupdataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowNodeScalarGroupDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowNodeScalarGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post2dWindowNodeScalarGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto container = item->topDataItem()->zoneDataItem()->dataContainer();
	if (! container->IBCExists()) {
		ui->rangeWidget->disableActive();
	}

	auto data = container->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(data);
	if (sgrid == nullptr) {
		ui->rangeWidget->hideCustom();
	} else {
		int dims[3];
		sgrid->GetDimensions(dims);
		ui->rangeWidget->setDimensions(dims[0], dims[1]);
	}

	auto cmw = new ColorMapSettingEditWidget(this);
	cmw->setSetting(&item->impl->m_setting.colorMapSetting);
	m_colorMapWidget = new ColorMapSettingEditWidgetWithImportExportButton(cmw, this);

	ui->colorMapWidget->setWidget(m_colorMapWidget);
	ui->rangeWidget->setSetting(&item->impl->m_setting.regionSetting);
	ui->contourWidget->setSetting(&item->impl->m_setting.contourSetting);
	ui->opacityWidget->setOpacityPercent(item->impl->m_setting.opacity);
}

Post2dWindowNodeScalarGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowNodeScalarGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowNodeScalarGroupDataItem::SetProperty"), apply);
	command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	command->addCommand(ui->rangeWidget->createModifyCommand());
	command->addCommand(ui->contourWidget->createModifyCommand(apply));

	OpacityContainer o;
	o.setValue(ui->opacityWidget->opacityPercent());
	command->addCommand(new ValueModifyCommmand<OpacityContainer>(iRIC::generateCommandId("ModifyOpacity"), apply, o, &m_item->impl->m_setting.opacity));

	return command;
}
