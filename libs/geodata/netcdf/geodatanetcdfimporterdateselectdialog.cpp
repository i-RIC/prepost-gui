#include "ui_geodatanetcdfimporterdateselectdialog.h"

#include "geodatanetcdfimporterdateselectdialog.h"

static QString defaultDisplayFormat("yyyy-MM-dd HH:mm:s.s");

GeoDataNetcdfImporterDateSelectDialog::GeoDataNetcdfImporterDateSelectDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataNetcdfImporterDateSelectDialog}
{
	ui->setupUi(this);
	ui->dateTimeEdit->setDisplayFormat(defaultDisplayFormat);
	ui->timeZoneComboBox->setTimeZone(QTimeZone::utc());
}

GeoDataNetcdfImporterDateSelectDialog::~GeoDataNetcdfImporterDateSelectDialog()
{
	delete ui;
}

void GeoDataNetcdfImporterDateSelectDialog::setUnit(const QString& unit)
{
	QRegExp rx("(.+) since (.+)");
	if (rx.indexIn(unit) != -1) {
		// matched
		QString units = rx.cap(1);
		ui->importUnitsAsComboBox->setCurrentText(units);
	}

	ui->unitsValueLabel->setText(unit);
}

void GeoDataNetcdfImporterDateSelectDialog::setOriginalDateTime(const QDateTime& orig)
{
	ui->dateTimeEdit->setDateTime(orig);
}

QDateTime GeoDataNetcdfImporterDateSelectDialog::originalDateTime() const
{
	return ui->dateTimeEdit->dateTime();
}

GeoDataNetcdfImporterDateSelectDialog::TimeUnit GeoDataNetcdfImporterDateSelectDialog::timeUnit() const
{
	std::vector<TimeUnit> units;
	units.push_back(TimeUnit::Years);
	units.push_back(TimeUnit::Days);
	units.push_back(TimeUnit::Hours);
	units.push_back(TimeUnit::Minutes);
	units.push_back(TimeUnit::Seconds);

	return units.at(ui->importUnitsAsComboBox->currentIndex());
}

QTimeZone GeoDataNetcdfImporterDateSelectDialog::timeZone() const
{
	return ui->timeZoneComboBox->timeZone();
}

int GeoDataNetcdfImporterDateSelectDialog::timeUnitScale(TimeUnit unit)
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
