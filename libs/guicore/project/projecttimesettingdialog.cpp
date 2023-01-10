#include "projecttimesettingdialog.h"
#include "ui_projecttimesettingdialog.h"

#include <QRegExp>
#include <QTimeZone>

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
		auto dt = ui->zeroTimeEdit->dateTime();
		dt.setTimeZone(timeZone());
		return dt.toUTC();
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

QTimeZone ProjectTimeSettingDialog::timeZone() const
{
	return ui->timeZoneComboBox->timeZone();
}

void ProjectTimeSettingDialog::setTimeZone(const QTimeZone& timeZone)
{
	ui->timeZoneComboBox->setTimeZone(timeZone);
	auto dt = ui->zeroTimeEdit->dateTime();
	dt.setTimeZone(QTimeZone::utc());
	auto dt2 = dt.toTimeZone(timeZone);
	ui->zeroTimeEdit->setDateTime(dt2);
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

bool ProjectTimeSettingDialog::showTimeZone() const
{
	return ui->timeFormatWidget->showTimeZone();
}

void ProjectTimeSettingDialog::setShowTimeZone(bool show)
{
	ui->timeFormatWidget->setShowTimeZone(show);
}
