#include "arrowssettingcontainer.h"
#include "arrowsshapesettingeditwidget.h"
#include "ui_arrowsshapesettingeditwidget.h"

ArrowsShapeSettingEditWidget::ArrowsShapeSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ArrowsShapeSettingEditWidget)
{
	ui->setupUi(this);
}

ArrowsShapeSettingEditWidget::~ArrowsShapeSettingEditWidget()
{
	delete ui;
}

ArrowsSettingContainer ArrowsShapeSettingEditWidget::setting() const
{
	ArrowsSettingContainer ret;
	updateSetting(&ret);
	return ret;
}

void ArrowsShapeSettingEditWidget::updateSetting(ArrowsSettingContainer* setting) const
{
	setting->arrowSize = ui->arrowSizeSpinBox->value();
	setting->lineWidth = ui->lineWidthSpinBox->value();
}

void ArrowsShapeSettingEditWidget::setSetting(const ArrowsSettingContainer& setting)
{
	ui->arrowSizeSpinBox->setValue(setting.arrowSize);
	ui->lineWidthSpinBox->setValue(setting.lineWidth);
}
