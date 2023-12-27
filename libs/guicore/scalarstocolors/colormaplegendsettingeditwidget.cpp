#include "colormapsettingcontainer.h"
#include "colormaplegendsettingcontainer.h"
#include "colormaplegendsettingeditwidget.h"
#include "ui_colormaplegendsettingeditwidget.h"

ColorMapLegendSettingEditWidget::ColorMapLegendSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_colorMapSetting {nullptr},
	m_delegateMode {false},
	ui(new Ui::ColorMapLegendSettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->numLabelsAutoCheckBox, &QCheckBox::toggled, this, &ColorMapLegendSettingEditWidget::handleAutoNumberOfLabels);
	connect(ui->horizontalRadioButton, &QRadioButton::toggled, this, &ColorMapLegendSettingEditWidget::reverseHeightAndWidthIfNeeded);
	connect(ui->verticalRadioButton, &QRadioButton::toggled, this, &ColorMapLegendSettingEditWidget::reverseHeightAndWidthIfNeeded);
}

ColorMapLegendSettingEditWidget::~ColorMapLegendSettingEditWidget()
{
	delete ui;
}

ColorMapLegendSettingContainer ColorMapLegendSettingEditWidget::setting() const
{
	ColorMapLegendSettingContainer ret;

	ret.visible = ui->visibleCheckBox->isChecked();
	if (ui->horizontalRadioButton->isChecked()) {
		ret.direction = ColorMapLegendSettingContainer::Direction::Horizontal;
	} else if (ui->verticalRadioButton->isChecked()) {
		ret.direction = ColorMapLegendSettingContainer::Direction::Vertical;
	}
	ret.barAutoWidth = ui->barWidthAutoCheckBox->isChecked();
	ret.barWidth = ui->barWidthSpinBox->value();
	if (ui->leftRadioButton->isChecked()) {
		ret.barAlign = ColorMapLegendSettingContainer::BarAlign::Left;
	} else if (ui->centerRadioButton->isChecked()) {
		ret.barAlign = ColorMapLegendSettingContainer::BarAlign::Center;
	} else if (ui->rightRadioButton->isChecked()) {
		ret.barAlign = ColorMapLegendSettingContainer::BarAlign::Right;
	}
	ret.title = ui->titleEdit->text();
	ret.labelFormat = ui->labelFormatEdit->text();
	ret.autoNumberOfLabels = ui->numLabelsAutoCheckBox->isChecked();
	ret.numberOfLabels = ui->numLabelsSpinBox->value();
	ret.labelSkipRate = ui->labelSkipRateSpinBox->value();
	ret.titleFont = ui->titleFontWidget->font();
	ret.labelFont = ui->labelFontWidget->font();
	ret.titleColor = ui->titleColorWidget->color();
	ret.labelColor = ui->labelColorWidget->color();
	ret.backgroundColor = ui->backgroundColorWidget->color();
	ret.backgroundOpacity = ui->backgroundOpacityWidget->opacity();
	ret.showBorder = ui->showBorderCheckBox->isChecked();
	ret.imageSetting = ui->imageSettingWidget->setting();

	ret.setDelegateMode(m_delegateMode);
	return ret;
}

void ColorMapLegendSettingEditWidget::setSetting(const ColorMapLegendSettingContainer& setting)
{
	m_delegateMode = setting.delegateMode();

	ColorMapLegendSettingContainer s;

	if (setting.delegateMode()) {
		s = setting.colorMapSetting()->legend;
		s.visible = setting.visible;
	} else {
		s.copyWithColorMap(setting);
	}
	s.setColorMapSetting(setting.colorMapSetting());

	ui->visibleCheckBox->setChecked(s.visible);
	if (s.direction == ColorMapLegendSettingContainer::Direction::Horizontal) {
		ui->horizontalRadioButton->setChecked(true);
	} else if (s.direction == ColorMapLegendSettingContainer::Direction::Vertical) {
		ui->verticalRadioButton->setChecked(true);
	}
	ui->barWidthAutoCheckBox->setChecked(s.barAutoWidth);
	ui->barWidthSpinBox->setValue(s.barWidth);
	if (s.barAlign == ColorMapLegendSettingContainer::BarAlign::Left) {
		ui->leftRadioButton->setChecked(true);
	} else if (s.barAlign == ColorMapLegendSettingContainer::BarAlign::Center) {
		ui->centerRadioButton->setChecked(true);
	} else if (s.barAlign == ColorMapLegendSettingContainer::BarAlign::Right) {
		ui->rightRadioButton->setChecked(true);
	}
	ui->titleEdit->setText(s.title);
	ui->labelFormatEdit->setText(s.labelFormat);
	ui->numLabelsAutoCheckBox->setChecked(s.autoNumberOfLabels);
	ui->numLabelsSpinBox->setValue(s.numberOfLabels);
	ui->labelSkipRateSpinBox->setValue(s.labelSkipRate);
	ui->titleFontWidget->setFont(s.titleFont);
	ui->labelFontWidget->setFont(s.labelFont);
	ui->titleColorWidget->setColor(s.titleColor);
	ui->labelColorWidget->setColor(s.labelColor);
	ui->backgroundColorWidget->setColor(s.backgroundColor);
	ui->backgroundOpacityWidget->setOpacity(s.backgroundOpacity);
	ui->showBorderCheckBox->setChecked(s.showBorder);

	ui->imageSettingWidget->setSetting(s.imageSetting);
	m_colorMapSetting = s.colorMapSetting();

	updateAutoNumberOfLabels();
	updateNumberOfLabelsIfNeeded();
	updateLabelNumberWidgetVisibility();
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

	int numCols = static_cast<int> (m_colorMapSetting->colors.size());
	if (m_colorMapSetting->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
		++ numCols;
	}
	ui->numLabelsSpinBox->setValue(numCols);
}

void ColorMapLegendSettingEditWidget::updateLabelNumberWidgetVisibility()
{
	if (m_colorMapSetting == nullptr) {return;}

	if (m_colorMapSetting->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		ui->numLabelsLabel->setVisible(true);
		ui->numLabelsAutoCheckBox->setVisible(true);
		ui->numLabelsSpinBox->setVisible(true);

		ui->skipRateLabel->setVisible(false);
		ui->labelSkipRateSpinBox->setVisible(false);
	} else if (m_colorMapSetting->transitionMode == ColorMapSettingContainer::TransitionMode::Discrete) {
		ui->numLabelsLabel->setVisible(false);
		ui->numLabelsAutoCheckBox->setVisible(false);
		ui->numLabelsSpinBox->setVisible(false);

		ui->skipRateLabel->setVisible(true);
		ui->labelSkipRateSpinBox->setVisible(true);
	}
}

void ColorMapLegendSettingEditWidget::setDisableOtherThanVisible(bool disabled)
{
	ui->directionGroupBox->setDisabled(disabled);
	ui->titleGroupBox->setDisabled(disabled);
	ui->labelGroupBox->setDisabled(disabled);
	ui->colorGroupBox->setDisabled(disabled);
	ui->imageSettingGroupBox->setDisabled(disabled);
	ui->barWidthGroupBox->setDisabled(disabled);
	ui->alignmentGroupBox->setDisabled(disabled);
}

void ColorMapLegendSettingEditWidget::setImageSetting(const ImageSettingContainer& setting)
{
	ui->imageSettingWidget->setSetting(setting);
}

void ColorMapLegendSettingEditWidget::handleAutoNumberOfLabels(bool /*checked*/)
{
	updateNumberOfLabelsIfNeeded();
}

void ColorMapLegendSettingEditWidget::reverseHeightAndWidthIfNeeded()
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
