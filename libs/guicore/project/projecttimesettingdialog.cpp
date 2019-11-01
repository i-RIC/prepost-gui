#include "projecttimesettingdialog.h"
#include "ui_projecttimesettingdialog.h"

ProjectTimeSettingDialog::ProjectTimeSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ProjectTimeSettingDialog)
{
	ui->setupUi(this);
	ui->timeFormatWidget->setActualTimeNotAvailable(true);
	connect(ui->zeroTimeDisableCheckBox, SIGNAL(toggled(bool)), ui->timeFormatWidget, SLOT(setActualTimeNotAvailable(bool)));
}

ProjectTimeSettingDialog::~ProjectTimeSettingDialog()
{
	delete ui;
}

QDateTime ProjectTimeSettingDialog::zeroDateTime() const
{
	if (ui->zeroTimeDisableCheckBox->isChecked()) {
		return QDateTime();
	} else {
		return ui->zeroTimeEdit->dateTime();
	}
}

void ProjectTimeSettingDialog::setZeroDateTime(const QDateTime& zeroDateTime)
{
	ui->zeroTimeDisableCheckBox->setChecked(zeroDateTime.isNull());
	if (zeroDateTime.isNull()) {
		auto dt = QDateTime::currentDateTime();
		auto t = dt.time();
		auto time_zerosec = QTime(t.hour(), t.minute());
		dt.setTime(time_zerosec);
		ui->zeroTimeEdit->setDateTime(dt);
	} else {
		ui->zeroTimeEdit->setDateTime(zeroDateTime);
	}
}

TimeFormat ProjectTimeSettingDialog::timeFormat() const
{
	return ui->timeFormatWidget->timeFormat();
}

void ProjectTimeSettingDialog::setTimeFormat(TimeFormat timeFormat)
{
	ui->timeFormatWidget->setTimeFormat(timeFormat);
}

QString ProjectTimeSettingDialog::customTimeFormat() const
{
	return ui->timeFormatWidget->customTimeFormat();
}

void ProjectTimeSettingDialog::setCustomTimeFormat(const QString& format)
{
	ui->timeFormatWidget->setCustomTimeFormat(format);
}
