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

void DoubleSpinBoxWithHelp::setMinimum(double min)
{
	ui->doubleSpinBox->setMinimum(min);
	updateHelp();
}

void DoubleSpinBoxWithHelp::setMaximum(double max)
{
	ui->doubleSpinBox->setMaximum(max);
	updateHelp();
}

void DoubleSpinBoxWithHelp::setValue(double value)
{
	ui->doubleSpinBox->setValue(value);
}

double DoubleSpinBoxWithHelp::value() const
{
	return ui->doubleSpinBox->value();
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
