#include "ui_doublespinboxwithhelp.h"

#include "doublespinboxwithhelp.h"

DoubleSpinBoxWithHelp::DoubleSpinBoxWithHelp(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::DoubleSpinBoxWithHelp)
{
	ui->setupUi(this);
}

DoubleSpinBoxWithHelp::~DoubleSpinBoxWithHelp()
{
	delete ui;
}


double DoubleSpinBoxWithHelp::minimum() const
{
	return ui->doubleSpinBox->minimum();
}

void DoubleSpinBoxWithHelp::setMinimum(double min)
{
	ui->doubleSpinBox->setMinimum(min);
	updateHelp();
}

double DoubleSpinBoxWithHelp::maximum() const
{
	return ui->doubleSpinBox->maximum();
}

void DoubleSpinBoxWithHelp::setMaximum(double max)
{
	ui->doubleSpinBox->setMaximum(max);
	updateHelp();
}

double DoubleSpinBoxWithHelp::value() const
{
	return ui->doubleSpinBox->value();
}

void DoubleSpinBoxWithHelp::setValue(double value)
{
	ui->doubleSpinBox->setValue(value);
}

void DoubleSpinBoxWithHelp::setAlignment(Qt::Alignment a)
{
	ui->doubleSpinBox->setAlignment(a);
}

void DoubleSpinBoxWithHelp::setDecimals(int prec)
{
	ui->doubleSpinBox->setDecimals(prec);
}


void DoubleSpinBoxWithHelp::updateHelp()
{
	QString msg = tr("Min: %1, Max: %2").arg(ui->doubleSpinBox->minimum()).arg(ui->doubleSpinBox->maximum());
	ui->helpLabel->setToolTip(msg);
}
