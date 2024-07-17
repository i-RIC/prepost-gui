#include "../post2dwindowabstractcellscalargrouptopdataitem.h"
#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindowabstractcellscalargroupdataitem_impl.h"
#include "post2dwindowabstractcellscalargroupdataitem_settingeditwidget.h"
#include "ui_post2dwindowabstractcellscalargroupdataitem_settingeditwidget.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowAbstractCellScalarGroupDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowAbstractCellScalarGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::Post2dWindowAbstractCellScalarGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto cont = item->topDataItem()->zoneDataItem()->v4DataContainer();
	if (! cont->gridData()->ibcExists(item->position())) {
		ui->rangeWidget->disableActive();
	}

	auto data = cont->gridData()->grid();
	if (dynamic_cast<v4Structured2dGrid*> (data) == nullptr) {
		ui->rangeWidget->hideCustom();
	} else {
		int dimI, dimJ;
		item->getDimensions(&dimI, &dimJ);
		ui->rangeWidget->setDimensions(dimI, dimJ);
	}
	auto output = item->topDataItem()->zoneDataItem()->gridTypeDataItem()->gridType()->output(item->target());

	auto cmw = output->createColorMapSettingEditWidget(this);
	cmw->setSetting(item->impl->m_setting.colorMapSetting);
	m_colorMapWidget = new ColorMapSettingEditWidgetWithImportExportButton(cmw, this);

	ui->colorMapWidget->setWidget(m_colorMapWidget);
	ui->rangeWidget->setSetting(&item->impl->m_setting.regionSetting);
	ui->gridCellSettingWidget->setSetting(&item->impl->m_setting.cellSetting);
}

Post2dWindowAbstractCellScalarGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

void Post2dWindowAbstractCellScalarGroupDataItem::SettingEditWidget::hideLineWidth()
{
	ui->gridCellSettingWidget->hideLineWidth();
}

QUndoCommand* Post2dWindowAbstractCellScalarGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowCellScalarGroupDataItem::SetProperty"), apply);
	command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	command->addCommand(ui->rangeWidget->createModifyCommand());
	command->addCommand(ui->gridCellSettingWidget->createModifyCommand(apply));

	return command;
}
