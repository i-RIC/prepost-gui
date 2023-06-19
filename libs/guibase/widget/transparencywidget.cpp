#include "ui_transparencywidget.h"

#include "transparencywidget.h"

#include <misc/opacitycontainer.h>

TransparencyWidget::TransparencyWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::TransparencyWidget)
{
	ui->setupUi(this);

	connect(ui->checkBox, &QCheckBox::toggled, [=]() {
		emit updated(opacity());
	});

	connect(ui->horizontalSlider, &SliderWithValue::valueChanged, [=]() {
		emit updated(opacity());
	});
}

TransparencyWidget::~TransparencyWidget()
{
	delete ui;
}

void TransparencyWidget::setOpacity(const OpacityContainer& newOpacity)
{
	auto oldOpacity = opacity();
	if (newOpacity == oldOpacity) {return;}

	ui->checkBox->setChecked(newOpacity.enabled);
	ui->horizontalSlider->setValue(100 - newOpacity.percent);
}

OpacityContainer TransparencyWidget::opacity() const
{
	OpacityContainer o;

	o.enabled = ui->checkBox->isChecked();
	o.percent = 100 - ui->horizontalSlider->value();

	return o;
}

void TransparencyWidget::setOpacityPercent(const int o)
{
	ui->checkBox->setChecked(true);
	ui->horizontalSlider->setValue(100 - o);
}

int TransparencyWidget::opacityPercent() const
{
	if (! ui->checkBox->isChecked()) {
		return 100;
	}

	return 100 - ui->horizontalSlider->value();
}
