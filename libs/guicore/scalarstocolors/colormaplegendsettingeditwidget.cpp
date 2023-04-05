#include "colormapsettingcontainer.h"
#include "colormaplegendsettingcontainer.h"
#include "colormaplegendsettingeditwidget.h"
#include "ui_colormaplegendsettingeditwidget.h"

ColorMapLegendSettingEditWidget::ColorMapLegendSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_colorMapSetting {nullptr},
	ui(new Ui::ColorMapLegendSettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->numLabelsAutoCheckBox, &QCheckBox::toggled, this, &ColorMapLegendSettingEditWidget::handleAutoNumberOfLabels);
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
	if (ui->horizontalRadioButton->isChecked()) {
		ret.direction = ColorMapLegendSettingContainer::Direction::Horizontal;
	} else if (ui->verticalRadioButton->isChecked()) {
		ret.direction = ColorMapLegendSettingContainer::Direction::Vertical;
	}
	ret.title = ui->titleEdit->text();
	ret.labelFormat = ui->labelFormatEdit->text();
	ret.autoNumberOfLabels = ui->numLabelsAutoCheckBox->isChecked();
	ret.numberOfLabels = ui->numLabelsSpinBox->value();
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
	if (setting.direction == ColorMapLegendSettingContainer::Direction::Horizontal) {
		ui->horizontalRadioButton->setChecked(true);
	} else if (setting.direction == ColorMapLegendSettingContainer::Direction::Vertical) {
		ui->verticalRadioButton->setChecked(true);
	}
	ui->titleEdit->setText(setting.title);
	ui->labelFormatEdit->setText(setting.labelFormat);
	ui->numLabelsAutoCheckBox->setChecked(setting.autoNumberOfLabels);
	ui->numLabelsSpinBox->setValue(setting.numberOfLabels);
	ui->titleFontWidget->setFont(setting.titleFont);
	ui->labelFontWidget->setFont(setting.labelFont);
	ui->titleColorWidget->setColor(setting.titleColor);
	ui->labelColorWidget->setColor(setting.labelColor);
	ui->backgroundColorWidget->setColor(setting.backgroundColor);
	ui->backgroundOpacitySlider->setOpacityPercent(setting.backgroundOpacity);

	ui->imageSettingWidget->setSetting(setting.imageSetting);
	m_colorMapSetting = setting.colorMapSetting();

	updateAutoNumberOfLabels();
	updateNumberOfLabelsIfNeeded();
}

void ColorMapLegendSettingEditWidget::updateAutoNumberOfLabels()
{
	if (m_colorMapSetting == nullptr) {return;}

	bool isDiscrete = (m_colorMapSetting->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete);
	ui->numLabelsAutoCheckBox->setDisabled(isDiscrete);
	if (isDiscrete) {
		ui->numLabelsAutoCheckBox->setChecked(true);
		updateNumberOfLabelsIfNeeded();
	}
}

void ColorMapLegendSettingEditWidget::updateNumberOfLabelsIfNeeded()
{
	if (! ui->numLabelsAutoCheckBox->isChecked()) {return;}
	if (m_colorMapSetting == nullptr) {return;}

	int numCols = m_colorMapSetting->colors.size();
	if (m_colorMapSetting->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
		++ numCols;
	}
	ui->numLabelsSpinBox->setValue(numCols);
}

void ColorMapLegendSettingEditWidget::setDisableOtherThanImageSetting(bool disabled)
{
	ui->visibilityGroupBox->setDisabled(disabled);
	ui->directionGroupBox->setDisabled(disabled);
	ui->titleGroupBox->setDisabled(disabled);
	ui->labelGroupBox->setDisabled(disabled);
	ui->colorGroupBox->setDisabled(disabled);
}

void ColorMapLegendSettingEditWidget::handleAutoNumberOfLabels(bool checked)
{
	updateNumberOfLabelsIfNeeded();
}
