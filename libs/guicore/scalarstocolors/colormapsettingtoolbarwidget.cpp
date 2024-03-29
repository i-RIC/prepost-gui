#include "colormapsettingcontainer.h"
#include "colormapsettingtoolbarwidget.h"
#include "ui_colormapsettingtoolbarwidget.h"

ColorMapSettingToolBarWidget::ColorMapSettingToolBarWidget(QWidget *parent) :
	QWidget(parent),
	m_updateWidthAndHeight {false},
	ui(new Ui::ColorMapSettingToolBarWidget)
{
	ui->setupUi(this);

	connect(ui->visibleCheckBox, &QCheckBox::toggled, [=](bool){emit updated();});
	connect(ui->horizontalRadioButton, &QRadioButton::clicked, this, &ColorMapSettingToolBarWidget::toggleUpdateWidthAndHeight);
	connect(ui->horizontalRadioButton, &QRadioButton::clicked, [=](bool){emit updated();});
	connect(ui->verticalRadioButton, &QRadioButton::clicked, this, &ColorMapSettingToolBarWidget::toggleUpdateWidthAndHeight);
	connect(ui->verticalRadioButton, &QRadioButton::clicked, [=](bool){emit updated();});
}

ColorMapSettingToolBarWidget::~ColorMapSettingToolBarWidget()
{
	delete ui;
}

void ColorMapSettingToolBarWidget::setSetting(ColorMapSettingContainerI* setting)
{
	m_setting = setting;
	connect(m_setting, &ColorMapSettingContainerI::updated, this, &ColorMapSettingToolBarWidget::applySetting);

	applySetting();
}

ColorMapSettingContainerI* ColorMapSettingToolBarWidget::modifiedSetting()
{
	auto setting = m_setting->copy();

	setting->legendSetting()->setVisible(ui->visibleCheckBox->isChecked());

	if (ui->horizontalRadioButton->isChecked()) {
		setting->legendSetting()->setDirection(ColorMapLegendSettingContainerI::Direction::Horizontal);
	} else if (ui->verticalRadioButton->isChecked()) {
		setting->legendSetting()->setDirection(ColorMapLegendSettingContainerI::Direction::Vertical);
	}

	if (m_updateWidthAndHeight) {
		int width = setting->legendSetting()->imgSetting()->width;
		int height = setting->legendSetting()->imgSetting()->height;

		if (ui->horizontalRadioButton->isChecked() && height > width) {
			setting->legendSetting()->imgSetting()->width = height;
			setting->legendSetting()->imgSetting()->height = width;
		}

		if (ui->verticalRadioButton->isChecked() && width > height) {
			setting->legendSetting()->imgSetting()->width = height;
			setting->legendSetting()->imgSetting()->height = width;
		}
	}

	m_updateWidthAndHeight = false;

	return setting;
}

bool ColorMapSettingToolBarWidget::visible() const
{
	return ui->visibleCheckBox->isChecked();
}

ColorMapLegendSettingContainerI::Direction ColorMapSettingToolBarWidget::direction() const
{
	if (ui->horizontalRadioButton->isChecked()) {
		return ColorMapLegendSettingContainerI::Direction::Horizontal;
	} else if (ui->verticalRadioButton->isChecked()) {
		return ColorMapLegendSettingContainerI::Direction::Vertical;
	}

	return ColorMapLegendSettingContainerI::Direction::Horizontal;
}

void ColorMapSettingToolBarWidget::applySetting()
{
	if (m_setting == nullptr) {return;}

	ui->visibleCheckBox->blockSignals(true);
	ui->horizontalRadioButton->blockSignals(true);
	ui->verticalRadioButton->blockSignals(true);

	ui->visibleCheckBox->setChecked(m_setting->legendSetting()->getVisible());
	ui->horizontalRadioButton->setEnabled(m_setting->legendSetting()->getVisible());
	ui->verticalRadioButton->setEnabled(m_setting->legendSetting()->getVisible());
	if (m_setting->legendSetting()->getDirection() == ColorMapLegendSettingContainerI::Direction::Horizontal) {
		ui->horizontalRadioButton->setChecked(true);
	}	else if (m_setting->legendSetting()->getDirection() == ColorMapLegendSettingContainerI::Direction::Vertical) {
		ui->verticalRadioButton->setChecked(true);
	}

	ui->visibleCheckBox->blockSignals(false);
	ui->horizontalRadioButton->blockSignals(false);
	ui->verticalRadioButton->blockSignals(false);
}

void ColorMapSettingToolBarWidget::toggleUpdateWidthAndHeight()
{
	m_updateWidthAndHeight = true;
}
