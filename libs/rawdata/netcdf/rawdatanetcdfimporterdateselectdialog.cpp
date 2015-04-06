#include "rawdatanetcdfimporterdateselectdialog.h"
#include "ui_rawdatanetcdfimporterdateselectdialog.h"

RawDataNetcdfImporterDateSelectDialog::RawDataNetcdfImporterDateSelectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RawDataNetcdfImporterDateSelectDialog)
{
	ui->setupUi(this);
}

RawDataNetcdfImporterDateSelectDialog::~RawDataNetcdfImporterDateSelectDialog()
{
	delete ui;
}

void RawDataNetcdfImporterDateSelectDialog::setUnit(const QString& unit)
{
	ui->unitsValueLabel->setText(unit);
}

void RawDataNetcdfImporterDateSelectDialog::setOriginalDateTime(const QDateTime& orig)
{
	ui->dateTimeEdit->setDateTime(orig);
}

QDateTime RawDataNetcdfImporterDateSelectDialog::originalDateTime() const
{
	return ui->dateTimeEdit->dateTime();
}
