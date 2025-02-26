#include "post3dwindowfacecontourgroupdataitem_setrangescommand.h"
#include "post3dwindowfacecontourgroupdataitem_settingeditwidget.h"
#include "ui_post3dwindowfacecontourgroupdataitem_settingeditwidget.h"
#include "../post3dwindowfacecontourgrouptopdataitem.h"
#include "../post3dwindowgridtypedataitem.h"
#include "../post3dwindowzonedataitem.h"

#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectdefaultcolormapsettings.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

Post3dWindowFaceContourGroupDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowFaceContourGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post3dWindowFaceContourGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto output = item->zoneDataItem()->gridTypeDataItem()->gridType()->output(item->target());

	auto cmw = output->createColorMapSettingEditWidget(this);
	cmw->setSetting(item->m_colorMapSetting);
	m_colorMapWidget = new ColorMapSettingEditWidgetWithImportExportButton(cmw, this);
	m_colorMapWidget->showSetAsDefaultButton();
	connect(m_colorMapWidget, &ColorMapSettingEditWidgetWithImportExportButton::setAsDefaultClicked, this, &SettingEditWidget::setAsDefault);

	ui->colorMapWidget->setWidget(m_colorMapWidget);

	auto grid = dynamic_cast<v4Structured3dGrid*> (m_item->data()->gridData()->grid());

	int dims[3];
	grid->vtkConcreteData()->concreteData()->GetDimensions(dims);
	auto pos = item->topDataItem()->position();
	if (pos == v4SolutionGrid::Position::IFace) {
		dims[0] += 1;
	} else if (pos == v4SolutionGrid::Position::JFace) {
		dims[1] += 1;
	} else if (pos == v4SolutionGrid::Position::KFace) {
		dims[2] += 1;
	}
	ui->rangeListWidget->setDimensions(dims);
	ui->rangeListWidget->setRanges(item->ranges());
}

Post3dWindowFaceContourGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowFaceContourGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post3dWindowFaceContourGroupDataItem::SettingEditWidget"), apply);
	command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	command->addCommand(new SetRangesCommand(ui->rangeListWidget->ranges(), m_item));

	return command;
}

void Post3dWindowFaceContourGroupDataItem::SettingEditWidget::setAsDefault()
{
	auto cmw = m_colorMapWidget->widget();
	m_item->projectData()->mainfile()->defaultColorMapSettings()->add(m_item->target(), cmw->copySetting());

	QMessageBox::information(this, tr("Information"), tr("Set as the default setting for this project."));
}

