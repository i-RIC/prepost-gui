#include "ui_transparencywidget.h"

#include "transparencywidget.h"

TransparencyWidget::TransparencyWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::TransparencyWidget)
{
	ui->setupUi(this);
	connect(ui->checkBox, SIGNAL(toggled(bool)), this, SLOT(handleCheck(bool)));
}

TransparencyWidget::~TransparencyWidget()
{
	delete ui;
}

void TransparencyWidget::setOpacityPercent(int opacityPercent)
{
	if (opacityPercent == 100) {
		ui->checkBox->setChecked(false);
		ui->horizontalSlider->setValue(0);
	} else {
		ui->checkBox->setChecked(true);
		ui->horizontalSlider->setValue(100 - opacityPercent);
	}
}

int TransparencyWidget::opacityPercent() const
{
	return 100 - ui->horizontalSlider->value();
}

void TransparencyWidget::handleCheck(bool checked)
{
	if (! checked) {
		ui->horizontalSlider->setValue(0);
	}
}
