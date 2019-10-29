#include "ui_posttimeeditdialog.h"

#include "posttimeeditdialog.h"

PostTimeEditDialog::PostTimeEditDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::PostTimeEditDialog}
{
	ui->setupUi(this);
}

PostTimeEditDialog::~PostTimeEditDialog()
{
	delete ui;
}

void PostTimeEditDialog::setSetting(const PostTimeDataItem::Setting& setting)
{
	ui->useProjectSettingCheckBox->setChecked(setting.useProjectSetting);
	ui->timeFormatEditWidget->setTimeFormat(setting.timeFormat.value());
	ui->timeFormatEditWidget->setCustomTimeFormat(setting.customTimeFormat.value());
	ui->fontSettingWidget->setSetting(setting.fontSetting);
}

PostTimeDataItem::Setting PostTimeEditDialog::setting() const
{
	PostTimeDataItem::Setting ret;
	ret.useProjectSetting = ui->useProjectSettingCheckBox->isChecked();
	ret.timeFormat = ui->timeFormatEditWidget->timeFormat();
	ret.customTimeFormat = ui->timeFormatEditWidget->customTimeFormat();
	ret.fontSetting = ui->fontSettingWidget->setting();
	return ret;
}

void PostTimeEditDialog::setActualTimeAvailable(bool available)
{
	ui->timeFormatEditWidget->setActualTimeAvailable(available);
}
