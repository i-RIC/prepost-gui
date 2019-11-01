#include "timeformatutil.h"
#include "timeformateditwidget.h"
#include "ui_timeformateditwidget.h"

TimeFormatEditWidget::TimeFormatEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::TimeFormatEditWidget)
{
	ui->setupUi(this);

	connect(ui->actualYYYYMMDDHHHHMMSSRadioButton, SIGNAL(toggled(bool)), this, SLOT(actual_yyyy_mm_dd_HH_MM_SS_toggled(bool)));
	connect(ui->actualYYYYMMDDHHHHMMRadioButton, SIGNAL(toggled(bool)), this, SLOT(actual_yyyy_mm_dd_HH_MM_toggled(bool)));
	connect(ui->actualMMDDHHMMSSRadioButton, SIGNAL(toggled(bool)), this, SLOT(actual_mm_dd_HH_MM_SS_toggled(bool)));
	connect(ui->actualMMDDHHMMRadioButton, SIGNAL(toggled(bool)), this, SLOT(actual_mm_dd_HH_MM_toggled(bool)));
	connect(ui->actualHHMMSSRadioButton, SIGNAL(toggled(bool)), this, SLOT(actual_HH_MM_SS_toggled(bool)));
	connect(ui->actualHHMMRadioButton, SIGNAL(toggled(bool)), this, SLOT(actual_HH_MM_toggled(bool)));
}

TimeFormatEditWidget::~TimeFormatEditWidget()
{
	delete ui;
}

void TimeFormatEditWidget::setActualTimeAvailable(bool available)
{
	if (available) {
		ui->showActualRadioButton->setEnabled(true);
	} else {
		ui->showActualRadioButton->setEnabled(false);
		ui->showElapsedRadioButton->setChecked(true);
	}
}

void TimeFormatEditWidget::setActualTimeNotAvailable(bool notAvailable)
{
	setActualTimeAvailable(! notAvailable);
}

TimeFormat TimeFormatEditWidget::timeFormat() const
{
	if (ui->showActualRadioButton->isChecked()) {
		if (ui->actualYYYYMMDDHHHHMMSSRadioButton->isChecked()) {
			return TimeFormat::actual_yyyy_mm_dd_HH_MM_SS;
		} else if (ui->actualYYYYMMDDHHHHMMRadioButton->isChecked()) {
			return TimeFormat::actual_yyyy_mm_dd_HH_MM;
		} else if (ui->actualMMDDHHMMSSRadioButton->isChecked()) {
			return TimeFormat::actual_mm_dd_HH_MM_SS;
		} else if (ui->actualMMDDHHMMRadioButton->isChecked()) {
			return TimeFormat::actual_mm_dd_HH_MM;
		} else if (ui->actualHHMMSSRadioButton->isChecked()) {
			return TimeFormat::actual_HH_MM_SS;
		} else if (ui->actualCustomRadioButton->isChecked()) {
			return TimeFormat::actual_Custom;
		} else {
			return TimeFormat::actual_HH_MM;
		}
	} else {
		if (ui->elapsedSSRadioButton->isChecked()) {
			return TimeFormat::elapsed_SS_sec;
		} else if (ui->elapsedMMSSRadioButton->isChecked()) {
			return TimeFormat::elapsed_MM_SS;
		} else if (ui->elapsedHHMMSSRadioButton->isChecked()) {
			return TimeFormat::elapsed_HH_MM_SS;
		} else if (ui->elapsedDDHHMMSSRadioButton->isChecked()) {
			return TimeFormat::elapsed_DD_HH_MM_SS;
		}
	}
}

void TimeFormatEditWidget::setTimeFormat(TimeFormat timeFormat)
{
	if (timeFormat == TimeFormat::actual_yyyy_mm_dd_HH_MM_SS) {
		ui->showActualRadioButton->setChecked(true);
		ui->actualYYYYMMDDHHHHMMSSRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::actual_yyyy_mm_dd_HH_MM) {
		ui->showActualRadioButton->setChecked(true);
		ui->actualYYYYMMDDHHHHMMRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::actual_mm_dd_HH_MM_SS) {
		ui->showActualRadioButton->setChecked(true);
		ui->actualMMDDHHMMSSRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::actual_mm_dd_HH_MM) {
		ui->showActualRadioButton->setChecked(true);
		ui->actualMMDDHHMMRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::actual_HH_MM_SS) {
		ui->showActualRadioButton->setChecked(true);
		ui->actualHHMMSSRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::actual_HH_MM) {
		ui->showActualRadioButton->setChecked(true);
		ui->actualHHMMRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::actual_Custom) {
		ui->showActualRadioButton->setChecked(true);
		ui->actualCustomRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::elapsed_SS_sec) {
		ui->showElapsedRadioButton->setChecked(true);
		ui->elapsedSSRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::elapsed_MM_SS) {
		ui->showElapsedRadioButton->setChecked(true);
		ui->elapsedMMSSRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::elapsed_HH_MM_SS) {
		ui->showElapsedRadioButton->setChecked(true);
		ui->elapsedHHMMSSRadioButton->setChecked(true);
	} else if (timeFormat == TimeFormat::elapsed_DD_HH_MM_SS) {
		ui->showElapsedRadioButton->setChecked(true);
		ui->elapsedDDHHMMSSRadioButton->setChecked(true);
	}
	ui->actualCustomLineEdit->setEnabled(ui->actualCustomRadioButton->isChecked());
}

QString TimeFormatEditWidget::customTimeFormat() const
{
	return ui->actualCustomLineEdit->text();
}

void TimeFormatEditWidget::setCustomTimeFormat(const QString& customFormat)
{
	ui->actualCustomLineEdit->setText(customFormat);
}

void TimeFormatEditWidget::actual_yyyy_mm_dd_HH_MM_SS_toggled(bool toggled)
{
	if (! toggled) {return;}

	ui->actualCustomLineEdit->setText("yyyy/MM/dd hh:mm:ss");
}

void TimeFormatEditWidget::actual_yyyy_mm_dd_HH_MM_toggled(bool toggled)
{
	if (! toggled) {return;}

	ui->actualCustomLineEdit->setText("yyyy/MM/dd hh:mm");
}

void TimeFormatEditWidget::actual_mm_dd_HH_MM_SS_toggled(bool toggled)
{
	if (! toggled) {return;}

	ui->actualCustomLineEdit->setText("MM/dd hh:mm:ss");
}

void TimeFormatEditWidget::actual_mm_dd_HH_MM_toggled(bool toggled)
{
	if (! toggled) {return;}

	ui->actualCustomLineEdit->setText("MM/dd hh:mm");
}

void TimeFormatEditWidget::actual_HH_MM_SS_toggled(bool toggled)
{
	if (! toggled) {return;}

	ui->actualCustomLineEdit->setText("hh:mm:ss");
}

void TimeFormatEditWidget::actual_HH_MM_toggled(bool toggled)
{
	if (! toggled) {return;}

	ui->actualCustomLineEdit->setText("hh:mm");
}
