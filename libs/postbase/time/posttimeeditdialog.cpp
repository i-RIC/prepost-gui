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
	switch (TimeFormat::Format(setting.timeFormat)) {
	case TimeFormat::Format::Second:
		ui->secondRadioButton->setChecked(true);
		break;
	case TimeFormat::Format::MinuteSecond:
		ui->minuteSecondRadioButton->setChecked(true);
		break;
	case TimeFormat::Format::HourMinuteSecond:
		ui->hourMinuteSecondRadioButton->setChecked(true);
		break;
	case TimeFormat::Format::DayHourMinuteSecond:
		ui->dayHourMinuteSecondRadioButton->setChecked(true);
		break;
	}
	ui->fontSettingWidget->setSetting(setting.fontSetting);
}

PostTimeDataItem::Setting PostTimeEditDialog::setting() const
{
	PostTimeDataItem::Setting ret;
	if (ui->secondRadioButton->isChecked()) {
		ret.timeFormat = TimeFormat::Format::Second;
	} else if (ui->minuteSecondRadioButton->isChecked()) {
		ret.timeFormat = TimeFormat::Format::MinuteSecond;
	} else if (ui->hourMinuteSecondRadioButton->isChecked()) {
		ret.timeFormat = TimeFormat::Format::HourMinuteSecond;
	} else {
		ret.timeFormat = TimeFormat::Format::DayHourMinuteSecond;
	}
	ret.fontSetting = ui->fontSettingWidget->setting();
	return ret;
}
