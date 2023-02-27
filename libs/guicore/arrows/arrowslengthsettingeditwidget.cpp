#include "arrowssettingcontainer.h"
#include "arrowslengthsettingeditwidget.h"
#include "ui_arrowslengthsettingeditwidget.h"

ArrowsLengthSettingEditWidget::ArrowsLengthSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ArrowsLengthSettingEditWidget)
{
	ui->setupUi(this);
}

ArrowsLengthSettingEditWidget::~ArrowsLengthSettingEditWidget()
{
	delete ui;
}

ArrowsSettingContainer ArrowsLengthSettingEditWidget::setting() const
{
	ArrowsSettingContainer ret;
	updateSetting(&ret);

	return ret;
}

void ArrowsLengthSettingEditWidget::updateSetting(ArrowsSettingContainer* setting) const
{
	if (ui->lengthAutoCheckBox->isChecked()) {
		setting->lengthMode = ArrowsSettingContainer::LengthMode::Auto;
	} else {
		setting->lengthMode = ArrowsSettingContainer::LengthMode::Custom;
	}
	setting->standardValue = ui->stdValueSpinBox->value();
	setting->legendLength = ui->legendLengthSpinBox->value();
	setting->minimumValue = ui->minValueSpinBox->value();
}
void ArrowsLengthSettingEditWidget::setSetting(const ArrowsSettingContainer& setting)
{
	ui->lengthAutoCheckBox->setChecked(setting.lengthMode == ArrowsSettingContainer::LengthMode::Auto);
	ui->stdValueSpinBox->setValue(setting.standardValue);
	ui->legendLengthSpinBox->setValue(setting.legendLength);
	ui->minValueSpinBox->setValue(setting.minimumValue);
}
