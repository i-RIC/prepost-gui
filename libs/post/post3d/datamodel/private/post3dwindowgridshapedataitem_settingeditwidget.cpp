#include "../post3dwindowzonedataitem.h"
#include "post3dwindowgridshapedataitem_settingeditwidget.h"
#include "ui_post3dwindowgridshapedataitem_settingeditwidget.h"

#include <guicore/postcontainer/postzonedatacontainer.h>

#include <vtkStructuredGrid.h>

Post3dWindowGridShapeDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowGridShapeDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post3dWindowGridShapeDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	auto grid = item->zoneDataItem()->dataContainer()->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(grid);
	if (sgrid == nullptr) {
		ui->shapeEditWidget->hideShape();
	}

	ui->shapeEditWidget->setSetting(&item->m_setting);
}

Post3dWindowGridShapeDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowGridShapeDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return ui->shapeEditWidget->createModifyCommand(apply);
}
