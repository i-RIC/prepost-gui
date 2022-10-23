#include "colormaplegendsettingcontainer.h"
#include "colormaplegendsettingeditwidget.h"
#include "ui_colormaplegendsettingeditwidget.h"

ColorMapLegendSettingEditWidget::ColorMapLegendSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ColorMapLegendSettingEditWidget)
{
	ui->setupUi(this);
}

ColorMapLegendSettingEditWidget::~ColorMapLegendSettingEditWidget()
{
	delete ui;
}

ColorMapLegendSettingContainer ColorMapLegendSettingEditWidget::setting() const
{
	ColorMapLegendSettingContainer ret;

	if (ui->visibilityWhenSelectedRadioButton->isChecked()) {
		ret.visibilityMode = ColorMapLegendSettingContainer::VisibilityMode::WhenSelected;
	} else if (ui->visibilityAlwaysRadioButton->isChecked()) {
		ret.visibilityMode = ColorMapLegendSettingContainer::VisibilityMode::Always;
	} else if (ui->visibilityNeverRadioButton->isChecked()) {
		ret.visibilityMode = ColorMapLegendSettingContainer::VisibilityMode::Never;
	}
	ret.title = ui->titleEdit->text();
	ret.labelFormat = ui->labelFormatEdit->text();
	ret.titleFont = ui->titleFontWidget->font();
	ret.labelFont = ui->labelFontWidget->font();
	ret.titleColor = ui->titleColorWidget->color();
	ret.labelColor = ui->labelColorWidget->color();
	ret.backgroundColor = ui->backgroundColorWidget->color();
	ret.backgroundOpacity = ui->backgroundOpacitySlider->opacityPercent();
	ret.imageSetting = ui->imageSettingWidget->setting();

	return ret;
}

void ColorMapLegendSettingEditWidget::setSetting(const ColorMapLegendSettingContainer& setting)
{
	if (setting.visibilityMode == ColorMapLegendSettingContainer::VisibilityMode::WhenSelected) {
		ui->visibilityWhenSelectedRadioButton->setChecked(true);
	} else if (setting.visibilityMode == ColorMapLegendSettingContainer::VisibilityMode::Always) {
		ui->visibilityAlwaysRadioButton->setChecked(true);
	} else if (setting.visibilityMode == ColorMapLegendSettingContainer::VisibilityMode::Never) {
		ui->visibilityNeverRadioButton->setChecked(true);
	}
	ui->titleEdit->setText(setting.title);
	ui->labelFormatEdit->setText(setting.labelFormat);
	ui->titleFontWidget->setFont(setting.titleFont);
	ui->labelFontWidget->setFont(setting.labelFont);
	ui->titleColorWidget->setColor(setting.titleColor);
	ui->labelColorWidget->setColor(setting.labelColor);
	ui->backgroundColorWidget->setColor(setting.backgroundColor);
	ui->backgroundOpacitySlider->setOpacityPercent(setting.backgroundOpacity);

	ui->imageSettingWidget->setSetting(setting.imageSetting);
}
