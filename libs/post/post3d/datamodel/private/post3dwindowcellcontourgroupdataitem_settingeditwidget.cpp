#include "post3dwindowcellcontourgroupdataitem_setrangescommand.h"
#include "post3dwindowcellcontourgroupdataitem_settingeditwidget.h"
#include "ui_post3dwindowcellcontourgroupdataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
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

	auto grid = vtkStructuredGrid::SafeDownCast(m_item->data()->data()->data());
	ui->rangeListWidget->setDimensions(grid->GetDimensions());
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
