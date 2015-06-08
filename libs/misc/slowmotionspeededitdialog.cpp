#include "ui_slowmotionspeededitdialog.h"

#include "slowmotionspeededitdialog.h"

SlowmotionSpeedEditDialog::SlowmotionSpeedEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::SlowmotionSpeedEditDialog)
{
	ui->setupUi(this);
}

SlowmotionSpeedEditDialog::~SlowmotionSpeedEditDialog()
{
	delete ui;
}

void SlowmotionSpeedEditDialog::setInterval(double interval)
{
	ui->speedSpinBox->setValue(interval);
}

double SlowmotionSpeedEditDialog::interval()
{
	return ui->speedSpinBox->value();
}
