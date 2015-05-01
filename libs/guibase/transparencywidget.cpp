#include "transparencywidget.h"
#include "ui_transparencywidget.h"

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

void TransparencyWidget::setOpacity(int opacity)
{
	if (opacity == 100) {
		ui->checkBox->setChecked(false);
		ui->horizontalSlider->setValue(0);
	} else {
		ui->checkBox->setChecked(true);
		ui->horizontalSlider->setValue(100 - opacity);
	}
}

int TransparencyWidget::opacity()
{
	return 100 - ui->horizontalSlider->value();
}

void TransparencyWidget::handleCheck(bool checked)
{
	if (! checked) {
		ui->horizontalSlider->setValue(0);
	}
}
