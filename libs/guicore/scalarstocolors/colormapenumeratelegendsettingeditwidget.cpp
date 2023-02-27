#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapenumeratelegendsettingeditwidget.h"

#include "ui_colormapenumeratelegendsettingeditwidget.h"

ColorMapEnumerateLegendSettingEditWidget::ColorMapEnumerateLegendSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ColorMapEnumerateLegendSettingEditWidget)
{
	ui->setupUi(this);
}

ColorMapEnumerateLegendSettingEditWidget::~ColorMapEnumerateLegendSettingEditWidget()
{
	delete ui;
}

ColorMapEnumerateLegendSettingContainer ColorMapEnumerateLegendSettingEditWidget::setting() const
{
	ColorMapEnumerateLegendSettingContainer ret;

	if (ui->visibilityWhenSelectedRadioButton->isChecked()) {
		ret.visibilityMode = ColorMapEnumerateLegendSettingContainer::VisibilityMode::WhenSelected;
	} else if (ui->visibilityAlwaysRadioButton->isChecked()) {
		ret.visibilityMode = ColorMapEnumerateLegendSettingContainer::VisibilityMode::Always;
	} else if (ui->visibilityNeverRadioButton->isChecked()) {
		ret.visibilityMode = ColorMapEnumerateLegendSettingContainer::VisibilityMode::Never;
	}
	ret.title = ui->titleEdit->text();
	ret.titleFont = ui->titleFontWidget->font();
	ret.labelFont = ui->labelFontWidget->font();
	ret.titleColor = ui->titleColorWidget->color();
	ret.labelColor = ui->labelColorWidget->color();
	ret.backgroundColor = ui->backgroundColorWidget->color();
	ret.backgroundOpacity = ui->backgroundOpacitySlider->opacityPercent();
	ret.imageSetting = ui->imageSettingWidget->setting();

	return ret;
}

void ColorMapEnumerateLegendSettingEditWidget::setSetting(const ColorMapEnumerateLegendSettingContainer& setting)
{
	if (setting.visibilityMode == ColorMapEnumerateLegendSettingContainer::VisibilityMode::WhenSelected) {
		ui->visibilityWhenSelectedRadioButton->setChecked(true);
	} else if (setting.visibilityMode == ColorMapEnumerateLegendSettingContainer::VisibilityMode::Always) {
		ui->visibilityAlwaysRadioButton->setChecked(true);
	} else if (setting.visibilityMode == ColorMapEnumerateLegendSettingContainer::VisibilityMode::Never) {
		ui->visibilityNeverRadioButton->setChecked(true);
	}
	ui->titleEdit->setText(setting.title);
	ui->titleFontWidget->setFont(setting.titleFont);
	ui->labelFontWidget->setFont(setting.labelFont);
	ui->titleColorWidget->setColor(setting.titleColor);
	ui->labelColorWidget->setColor(setting.labelColor);
	ui->backgroundColorWidget->setColor(setting.backgroundColor);
	ui->backgroundOpacitySlider->setOpacityPercent(setting.backgroundOpacity);

	ui->imageSettingWidget->setSetting(setting.imageSetting);
}
