#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindownodescalargrouptopdataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodescalargroupdataitem_impl.h"
#include "post2dwindownodescalargroupdataitem_settingeditwidget.h"
#include "ui_post2dwindownodescalargroupdataitem_settingeditwidget.h"

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

Post2dWindowNodeScalarGroupDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowNodeScalarGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post2dWindowNodeScalarGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto cont = item->topDataItem()->zoneDataItem()->v4DataContainer();
	if (! cont->gridData()->ibcExists(v4SolutionGrid::Position::Node)) {
		ui->rangeWidget->disableActive();
	}

	auto data = cont->gridData()->grid();
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (data);
	if (sGrid == nullptr) {
		ui->rangeWidget->hideCustom();
	} else {
		int dims[3];
		sGrid->vtkConcreteData()->concreteData()->GetDimensions(dims);
		ui->rangeWidget->setDimensions(dims[0], dims[1]);
	}
	auto output = item->topDataItem()->zoneDataItem()->gridTypeDataItem()->gridType()->output(item->target());

	auto cmw = output->createColorMapSettingEditWidget(this);
	cmw->setSetting(item->impl->m_setting.colorMapSetting);
	m_colorMapWidget = new ColorMapSettingEditWidgetWithImportExportButton(cmw, this);

	ui->colorMapWidget->setWidget(m_colorMapWidget);
	ui->rangeWidget->setSetting(&item->impl->m_setting.regionSetting);
	ui->contourWidget->setSetting(&item->impl->m_setting.contourSetting);
	ui->opacityWidget->setOpacity(item->impl->m_setting.opacity);

	auto cs = dynamic_cast<ColorMapSettingContainer*> (item->impl->m_setting.colorMapSetting);
	if (cs == nullptr) {
		ui->contourWidget->disable();
	}
}

Post2dWindowNodeScalarGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowNodeScalarGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowV4NodeScalarGroupDataItem::SetProperty"), apply);
	command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	command->addCommand(ui->rangeWidget->createModifyCommand());
	command->addCommand(ui->contourWidget->createModifyCommand(apply));

	OpacityContainer o = ui->opacityWidget->opacity();
	command->addCommand(new ValueModifyCommmand<OpacityContainer>(iRIC::generateCommandId("ModifyOpacity"), apply, o, &m_item->impl->m_setting.opacity));

	return command;
}
