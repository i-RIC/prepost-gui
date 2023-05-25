#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapenumeratelegendsettingeditwidget.h"
#include "colormapenumeratesettingcontainer.h"

#include "ui_colormapenumeratelegendsettingeditwidget.h"

ColorMapEnumerateLegendSettingEditWidget::ColorMapEnumerateLegendSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_delegateMode {false},
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

	ret.visible = ui->visibleCheckBox->isChecked();
	if (ui->horizontalRadioButton->isChecked()) {
		ret.direction = ColorMapEnumerateLegendSettingContainer::Direction::Horizontal;
	} else if (ui->verticalRadioButton->isChecked()) {
		ret.direction = ColorMapEnumerateLegendSettingContainer::Direction::Vertical;
	}
	ret.barAutoWidth = ui->barWidthAutoCheckBox->isChecked();
	ret.barWidth = ui->barWidthSpinBox->value();
	if (ui->leftRadioButton->isChecked()) {
		ret.barAlign = ColorMapEnumerateLegendSettingContainer::BarAlign::Left;
	} else if (ui->centerRadioButton->isChecked()) {
		ret.barAlign = ColorMapEnumerateLegendSettingContainer::BarAlign::Center;
	} else if (ui->rightRadioButton->isChecked()) {
		ret.barAlign = ColorMapEnumerateLegendSettingContainer::BarAlign::Right;
	}
	ret.title = ui->titleEdit->text();
	ret.titleFont = ui->titleFontWidget->font();
	ret.labelFont = ui->labelFontWidget->font();
	ret.titleColor = ui->titleColorWidget->color();
	ret.labelColor = ui->labelColorWidget->color();
	ret.backgroundColor = ui->backgroundColorWidget->color();
	ret.backgroundOpacity = ui->backgroundOpacitySlider->opacityPercent();
	ret.showBorder = ui->showBorderCheckBox->isChecked();
	ret.imageSetting = ui->imageSettingWidget->setting();

	ret.setDelegateMode(m_delegateMode);
	return ret;
}

void ColorMapEnumerateLegendSettingEditWidget::setSetting(const ColorMapEnumerateLegendSettingContainer& setting)
{
	m_delegateMode = setting.delegateMode();

	ColorMapEnumerateLegendSettingContainer s;

	if (setting.delegateMode()) {
		s = setting.colorMapSetting()->legend;
		s.visible = setting.visible;
	} else {
		s = setting;
	}

	ui->visibleCheckBox->setChecked(s.visible);
	if (s.direction == ColorMapEnumerateLegendSettingContainer::Direction::Horizontal) {
		ui->horizontalRadioButton->setChecked(true);
	} else if (s.direction == ColorMapEnumerateLegendSettingContainer::Direction::Vertical) {
		ui->verticalRadioButton->setChecked(true);
	}
	ui->barWidthAutoCheckBox->setChecked(s.barAutoWidth);
	ui->barWidthSpinBox->setValue(s.barWidth);
	if (s.barAlign == ColorMapEnumerateLegendSettingContainer::BarAlign::Left) {
		ui->leftRadioButton->setChecked(true);
	} else if (s.barAlign == ColorMapEnumerateLegendSettingContainer::BarAlign::Center) {
		ui->centerRadioButton->setChecked(true);
	} else if (s.barAlign == ColorMapEnumerateLegendSettingContainer::BarAlign::Right) {
		ui->rightRadioButton->setChecked(true);
	}
	ui->titleEdit->setText(s.title);
	ui->titleFontWidget->setFont(s.titleFont);
	ui->labelFontWidget->setFont(s.labelFont);
	ui->titleColorWidget->setColor(s.titleColor);
	ui->labelColorWidget->setColor(s.labelColor);
	ui->backgroundColorWidget->setColor(s.backgroundColor);
	ui->backgroundOpacitySlider->setOpacityPercent(s.backgroundOpacity);
	ui->showBorderCheckBox->setChecked(s.showBorder);

	ui->imageSettingWidget->setSetting(s.imageSetting);
}

void ColorMapEnumerateLegendSettingEditWidget::setImageSetting(const ImageSettingContainer& setting)
{
	ui->imageSettingWidget->setSetting(setting);
}

void ColorMapEnumerateLegendSettingEditWidget::setDisableOtherThanImageVisible(bool disabled)
{
	ui->titleGroupBox->setDisabled(disabled);
	ui->labelGroupBox->setDisabled(disabled);
	ui->colorGroupBox->setDisabled(disabled);
	ui->imageSettingGroupBox->setDisabled(disabled);
	ui->barWidthGroupBox->setDisabled(disabled);
	ui->alignmentGroupBox->setDisabled(disabled);
}

void ColorMapEnumerateLegendSettingEditWidget::reverseHeightAndWidthIfNeeded()
{
	int height = ui->imageSettingWidget->height();
	int width = ui->imageSettingWidget->width();

	if (ui->horizontalRadioButton->isChecked()) {
		if (height > width) {
			ui->imageSettingWidget->setWidth(height);
			ui->imageSettingWidget->setHeight(width);
		}
	} else if (ui->verticalRadioButton->isChecked()) {
		if (width > height) {
			ui->imageSettingWidget->setWidth(height);
			ui->imageSettingWidget->setHeight(width);
		}
	}
}
