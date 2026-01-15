#include "post2dwindowbcdataitem_impl.h"
#include "post2dwindowbcdataitem_setting.h"
#include "post2dwindowbcdataitem_settingeditwidget.h"
#include "ui_post2dwindowbcdataitem_settingeditwidget.h"

#include <guicore/postcontainer/v4postzonedatabc.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowBCDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowBCDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::Post2dWindowBCDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	ui->typeWidget->setText(item->impl->m_inputGridBC->condition()->caption());
	ui->nameWidget->setText(item->impl->m_inputGridBC->caption().c_str());

	setSetting(item->impl->m_setting);
}

Post2dWindowBCDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowBCDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<Setting>(iRIC::generateCommandId("Post2dWindowBCDataItem::SettingEditWidget"),
		apply, setting(), &m_item->impl->m_setting);
}

Post2dWindowBCDataItem::Setting Post2dWindowBCDataItem::SettingEditWidget::setting()
{
	Setting ret;

	ret.opacity = ui->transparencyWidget->opacity();
	ret.color = ui->colorWidget->color();
	ret.showName = ui->showNameCheckBox->isChecked();
	ret.pointSize = ui->pointSizeSpinBox->value();

	return ret;
}

void Post2dWindowBCDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->transparencyWidget->setOpacity(setting.opacity);
	ui->colorWidget->setColor(setting.color);
	ui->showNameCheckBox->setChecked(setting.showName);
	ui->pointSizeSpinBox->setValue(setting.pointSize);
}