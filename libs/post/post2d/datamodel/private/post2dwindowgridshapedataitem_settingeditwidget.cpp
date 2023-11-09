#include "../post2dwindowgriddataitemi.h"
#include "post2dwindowgridshapedataitem_settingeditwidget.h"
#include "ui_post2dwindowgridshapedataitem_settingeditwidget.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowGridShapeDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowGridShapeDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_item {item},
	ui(new Ui::Post2dWindowGridShapeDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	ui->shapeEditWidget->setSetting(&item->m_setting);

	auto grid = item->gridDataItem()->grid();
	auto sGrid = dynamic_cast<v4Structured2dGrid*>(grid);
	if (sGrid == nullptr) {
		ui->shapeEditWidget->hideShape();
	}
}

Post2dWindowGridShapeDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowGridShapeDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return ui->shapeEditWidget->createModifyCommand(apply);
}
