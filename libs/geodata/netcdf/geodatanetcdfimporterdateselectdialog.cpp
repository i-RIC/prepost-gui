#include "ui_geodatanetcdfimporterdateselectdialog.h"

#include "geodatanetcdfimporterdateselectdialog.h"

GeoDataNetcdfImporterDateSelectDialog::GeoDataNetcdfImporterDateSelectDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataNetcdfImporterDateSelectDialog}
{
	ui->setupUi(this);
}

GeoDataNetcdfImporterDateSelectDialog::~GeoDataNetcdfImporterDateSelectDialog()
{
	delete ui;
}

void GeoDataNetcdfImporterDateSelectDialog::setUnit(const QString& unit)
{
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
