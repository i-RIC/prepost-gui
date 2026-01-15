#include "post2dwindowbcdataitem_settingeditwidget.h"
#include "ui_post2dwindowbcdataitem_settingeditwidget.h"

Post2dWindowBCDataItem::SettingEditWidget::SettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	ui(new Ui::Post2dWindowBCDataItem_SettingEditWidget)
{
	ui->setupUi(this);
}

Post2dWindowBCDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}
