#include "posttimedataitem_editwidget.h"
#include "ui_posttimedataitem_editwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

PostTimeDataItem::EditWidget::EditWidget(PostTimeDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::PostTimeDataItem_EditWidget)
{
	ui->setupUi(this);

	setSetting(item->m_setting);
}

PostTimeDataItem::EditWidget::~EditWidget()
{
	delete ui;
}

PostTimeDataItem::Setting PostTimeDataItem::EditWidget::setting() const
{
	Setting ret;

	ret.useProjectSetting = ui->useProjectSettingCheckBox->isChecked();
	ret.timeFormat = ui->timeFormatEditWidget->timeFormat();
	ret.customTimeFormat = ui->timeFormatEditWidget->customTimeFormat();
	ret.showTimeZone = ui->timeFormatEditWidget->showTimeZone();
	ret.fontSetting = ui->fontSettingWidget->setting();

	return ret;
}

void PostTimeDataItem::EditWidget::setSetting(const Setting& setting)
{
	ui->useProjectSettingCheckBox->setChecked(setting.useProjectSetting);
	ui->timeFormatEditWidget->setTimeFormat(setting.timeFormat.value());
	ui->timeFormatEditWidget->setCustomTimeFormat(setting.customTimeFormat.value());
	ui->timeFormatEditWidget->setShowTimeZone(setting.showTimeZone);
	ui->fontSettingWidget->setSetting(setting.fontSetting);
}

void PostTimeDataItem::EditWidget::setActualTimeAvailable(bool available)
{
	ui->timeFormatEditWidget->setActualTimeAvailable(available);
}

QUndoCommand* PostTimeDataItem::EditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<Setting> (iRIC::generateCommandId("PostTimeDataItem::EditWidget"), apply, setting(), &m_item->m_setting);
}
