#include "../post2dwindowzonedataitem.h"
#include "post2dwindowgridshapedataitem_settingeditwidget.h"
#include "ui_post2dwindowgridshapedataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <vtkStructuredGrid.h>

Post2dWindowGridShapeDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowGridShapeDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post2dWindowGridShapeDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto grid = item->zoneDataItem()->dataContainer()->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(grid);
	if (sgrid == nullptr) {
		ui->shapeEditWidget->hideShape();
	}

	ui->shapeEditWidget->setSetting(&item->m_setting);
}

Post2dWindowGridShapeDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowGridShapeDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return ui->shapeEditWidget->createModifyCommand(apply);
}
