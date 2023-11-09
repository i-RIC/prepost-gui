#include "post3dwindowcellcontourgroupdataitem_setrangescommand.h"
#include "post3dwindowcellcontourgroupdataitem_settingeditwidget.h"
#include "ui_post3dwindowcellcontourgroupdataitem_settingeditwidget.h"

#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

Post3dWindowCellContourGroupDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowCellContourGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	m_colorMapWidget {new ColorMapSettingEditWidget(this)},
	ui(new Ui::Post3dWindowCellContourGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto w = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapWidget, this);
	ui->colorMapWidget->setWidget(w);

	m_colorMapWidget->setSetting(&item->m_colorMapSetting);

	auto grid = dynamic_cast<v4Structured3dGrid*> (m_item->data()->gridData()->grid());
	ui->rangeListWidget->setDimensions(grid->vtkConcreteData()->concreteData()->GetDimensions());
	ui->rangeListWidget->setRanges(item->ranges());
}

Post3dWindowCellContourGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowCellContourGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post3dWindowCellContourGroupDataItem::SettingEditWidget"), apply);
	command->addCommand(m_colorMapWidget->createModifyCommand());
	command->addCommand(new SetRangesCommand(ui->rangeListWidget->ranges(), m_item));

	return command;
}
