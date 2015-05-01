#include "posttimeeditdialog.h"
#include "ui_posttimeeditdialog.h"

PostTimeEditDialog::PostTimeEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PostTimeEditDialog)
{
	ui->setupUi(this);
}

PostTimeEditDialog::~PostTimeEditDialog()
{
	delete ui;
}

void PostTimeEditDialog::setSetting(const PostTimeSetting& setting)
{
	switch (setting.timeFormat) {
	case TimeFormat::fmSecond:
		ui->secondRadioButton->setChecked(true);
		break;
	case TimeFormat::fmMinuteSecond:
		ui->minuteSecondRadioButton->setChecked(true);
		break;
	case TimeFormat::fmHourMinuteSecond:
		ui->hourMinuteSecondRadioButton->setChecked(true);
		break;
	case TimeFormat::fmDayHourMinuteSecond:
		ui->dayHourMinuteSecondRadioButton->setChecked(true);
		break;
	}
	ui->colorWidget->setColor(setting.color);
}

const PostTimeSetting PostTimeEditDialog::setting() const
{
	PostTimeSetting ret;
	if (ui->secondRadioButton->isChecked()) {
		ret.timeFormat = TimeFormat::fmSecond;
	} else if (ui->minuteSecondRadioButton->isChecked()) {
		ret.timeFormat = TimeFormat::fmMinuteSecond;
	} else if (ui->hourMinuteSecondRadioButton->isChecked()) {
		ret.timeFormat = TimeFormat::fmHourMinuteSecond;
	} else {
		ret.timeFormat = TimeFormat::fmDayHourMinuteSecond;
	}
	ret.color = ui->colorWidget->color();
	return ret;
}
