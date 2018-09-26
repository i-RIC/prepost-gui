#include "ui_geodatanetcdfimporterdateselectdialog.h"

#include "geodatanetcdfimporterdateselectdialog.h"

static QString defaultDisplayFormat("yyyy-MM-dd HH:mm:s.s");

GeoDataNetcdfImporterDateSelectDialog::GeoDataNetcdfImporterDateSelectDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataNetcdfImporterDateSelectDialog}
{
	ui->setupUi(this);
	ui->dateTimeEdit->setDisplayFormat(defaultDisplayFormat);
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

QString GeoDataNetcdfImporterDateSelectDialog::unit() const
{
	return ui->importUnitsAsComboBox->currentText();
}
