#include "ui_geodatagdalimporterdateselectdialog.h"

#include "geodatagdalimporterdateselectdialog.h"

static QString defaultDisplayFormat("yyyy-MM-dd HH:mm:s.s");

GeoDataGdalImporterDateSelectDialog::GeoDataGdalImporterDateSelectDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataGdalImporterDateSelectDialog}
{
	ui->setupUi(this);
	ui->dateTimeEdit->setDisplayFormat(defaultDisplayFormat);
	ui->timeZoneComboBox->setTimeZone(QTimeZone::utc());
}

GeoDataGdalImporterDateSelectDialog::~GeoDataGdalImporterDateSelectDialog()
{
	delete ui;
}

void GeoDataGdalImporterDateSelectDialog::setUnit(const QString& unit)
{
	QRegExp rx("(.+) since (.+)");
	if (rx.indexIn(unit) != -1) {
		// matched
		QString units = rx.cap(1);
		ui->importUnitsAsComboBox->setCurrentText(units);
	}

	ui->unitsValueLabel->setText(unit);
}

void GeoDataGdalImporterDateSelectDialog::setOriginalDateTime(const QDateTime& orig)
{
	ui->dateTimeEdit->setDateTime(orig);
}

QDateTime GeoDataGdalImporterDateSelectDialog::originalDateTime() const
{
	return ui->dateTimeEdit->dateTime();
}

GeoDataGdalImporterDateSelectDialog::TimeUnit GeoDataGdalImporterDateSelectDialog::timeUnit() const
{
	std::vector<TimeUnit> units;
	units.push_back(TimeUnit::Years);
	units.push_back(TimeUnit::Days);
	units.push_back(TimeUnit::Hours);
	units.push_back(TimeUnit::Minutes);
	units.push_back(TimeUnit::Seconds);

	return units.at(ui->importUnitsAsComboBox->currentIndex());
}

QTimeZone GeoDataGdalImporterDateSelectDialog::timeZone() const
{
	return ui->timeZoneComboBox->timeZone();
}

int GeoDataGdalImporterDateSelectDialog::timeUnitScale(TimeUnit unit)
{
	if (unit == TimeUnit::Years) {
		return 60 * 60 * 24 * 365;
	} else if (unit == TimeUnit::Days) {
		return 60 * 60 * 24;
	} else if (unit == TimeUnit::Hours) {
		return 60 * 60;
	} else if (unit == TimeUnit::Minutes) {
		return 60;
	} else if (unit == TimeUnit::Seconds) {
		return 1;
	}

	return 1;
}
