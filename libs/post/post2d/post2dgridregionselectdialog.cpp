#include "ui_post2dgridregionselectdialog.h"

#include "post2dgridregionselectdialog.h"

Post2dGridRegionSelectDialog::Post2dGridRegionSelectDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dGridRegionSelectDialog)
{
	ui->setupUi(this);
}

Post2dGridRegionSelectDialog::~Post2dGridRegionSelectDialog()
{
	delete ui;
}

void Post2dGridRegionSelectDialog::disableActive()
{
	ui->activeRadioButton->setDisabled(true);
}

void Post2dGridRegionSelectDialog::hideCustom()
{
	ui->customRadioButton->hide();
	ui->structuredGridRegionWidget->hide();
	adjustSize();
}

void Post2dGridRegionSelectDialog::setRegionMode(StructuredGridRegion::RegionMode rm)
{
	if (rm == StructuredGridRegion::rmFull) {
		ui->fullRadioButton->setChecked(true);
		ui->structuredGridRegionWidget->selectAll();
	} else if (rm == StructuredGridRegion::rmActive) {
		ui->activeRadioButton->setChecked(true);
		ui->structuredGridRegionWidget->selectAll();
	} else {
		ui->customRadioButton->setChecked(true);
	}
}

StructuredGridRegion::RegionMode Post2dGridRegionSelectDialog::regionMode()
{
	if (ui->fullRadioButton->isChecked()) {
		return StructuredGridRegion::rmFull;
	} else if (ui->activeRadioButton->isChecked()) {
		return StructuredGridRegion::rmActive;
	} else {
		return StructuredGridRegion::rmCustom;
	}
}

StructuredGridRegion::Range2d Post2dGridRegionSelectDialog::region()
{
	return ui->structuredGridRegionWidget->region();
}

void Post2dGridRegionSelectDialog::setRegion(const StructuredGridRegion::Range2d& region)
{
	if (ui->customRadioButton->isChecked()) {
		ui->structuredGridRegionWidget->setRegion(region);
	}
}

void Post2dGridRegionSelectDialog::setGridSize(int isize, int jsize)
{
	ui->structuredGridRegionWidget->setGridDimensions(isize, jsize);
}
