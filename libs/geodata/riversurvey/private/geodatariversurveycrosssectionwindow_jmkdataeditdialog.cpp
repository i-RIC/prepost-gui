#include "geodatariversurveycrosssectionwindow_jmkdataeditdialog.h"
#include "ui_geodatariversurveycrosssectionwindow_jmkdataeditdialog.h"

GeoDataRiverSurveyCrosssectionWindow::JmkDataEditDialog::JmkDataEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyCrosssectionWindow_JmkDataEditDialog)
{
	ui->setupUi(this);
}

GeoDataRiverSurveyCrosssectionWindow::JmkDataEditDialog::~JmkDataEditDialog()
{
	delete ui;
}

GeoDataRiverPathPointJmkData::Item GeoDataRiverSurveyCrosssectionWindow::JmkDataEditDialog::item() const
{
	GeoDataRiverPathPointJmkData::Item ret;

	ret.distance = ui->distanceEdit->value();
	ret.width = ui->widthEdit->value();
	ret.height = ui->heightEdit->value();
	int submerged = 0;
	if (ui->submergedCheckBox->isChecked()) {
		submerged = 1;
	}
	ret.submerged = submerged;
	ret.dense = ui->sparseDenseComboBox->currentIndex();
	ret.dead = ui->deadComboBox->currentIndex();
	ret.highLow = ui->highLowComboBox->currentIndex();
	ret.lowBranchHeight = ui->lowBranchHeightEdit->value();

	return ret;
}

void GeoDataRiverSurveyCrosssectionWindow::JmkDataEditDialog::setItem(const GeoDataRiverPathPointJmkData::Item& item)
{
	ui->distanceEdit->setValue(item.distance);
	ui->widthEdit->setValue(item.width);
	ui->heightEdit->setValue(item.height);
	ui->submergedCheckBox->setChecked(item.submerged == 1);
	ui->sparseDenseComboBox->setCurrentIndex(item.dense);
	ui->deadComboBox->setCurrentIndex(item.dead);
	ui->highLowComboBox->setCurrentIndex(item.highLow);
	ui->lowBranchHeightEdit->setValue(item.lowBranchHeight);
}
