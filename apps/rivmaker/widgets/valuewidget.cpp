#include "valuewidget.h"
#include "ui_valuewidget.h"

ValueWidget::ValueWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ValueWidget)
{
	ui->setupUi(this);
}

ValueWidget::~ValueWidget()
{
	delete ui;
}

void ValueWidget::setValue(double val)
{
	ui->labelVal->setText(QString("Value: %1").arg(val, 0, 'f', 3));
}

void ValueWidget::clear()
{
	ui->labelVal->setText(tr("Value: ------"));
}
