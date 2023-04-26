#include "colormapsettingcontainer.h"
#include "colormapsettingtoolbarwidget.h"
#include "ui_colormapsettingtoolbarwidget.h"

ColorMapSettingToolBarWidget::ColorMapSettingToolBarWidget(QWidget *parent) :
	ColorMapSettingToolBarWidgetI(parent),
	ui(new Ui::ColorMapSettingToolBarWidget)
{
	ui->setupUi(this);

	connect(ui->visibleCheckBox, &QCheckBox::toggled, [=](bool){emit updated();});
	connect(ui->horizontalRadioButton, &QCheckBox::clicked, [=](bool){emit updated();});
	connect(ui->verticalRadioButton, &QCheckBox::clicked, [=](bool){emit updated();});
}

ColorMapSettingToolBarWidget::~ColorMapSettingToolBarWidget()
{
	delete ui;
}

ColorMapSettingContainerI* ColorMapSettingToolBarWidget::modifiedSetting()
{
	auto setting = new ColorMapSettingContainer();
	*setting = *(dynamic_cast<ColorMapSettingContainer*> (m_setting));

	setting->legend.visible = ui->visibleCheckBox->isChecked();

	int width = setting->legend.imageSetting.width;
	int height = setting->legend.imageSetting.height;
	if (ui->horizontalRadioButton->isChecked()) {
		setting->legend.direction = ColorMapLegendSettingContainer::Direction::Horizontal;
		if (height > width) {
			setting->legend.imageSetting.width = height;
			setting->legend.imageSetting.height = width;
		}
	} else if (ui->verticalRadioButton->isChecked()) {
		setting->legend.direction = ColorMapLegendSettingContainer::Direction::Vertical;
		if (width > height) {
			setting->legend.imageSetting.width = height;
			setting->legend.imageSetting.height = width;
		}
	}

	return setting;
}

bool ColorMapSettingToolBarWidget::visible() const
{
	return ui->visibleCheckBox->isChecked();
}

ColorMapLegendSettingContainer::Direction ColorMapSettingToolBarWidget::direction() const
{
	if (ui->horizontalRadioButton->isChecked()) {
		return ColorMapLegendSettingContainer::Direction::Horizontal;
	} else if (ui->verticalRadioButton->isChecked()) {
		return ColorMapLegendSettingContainer::Direction::Vertical;
	}

	return ColorMapLegendSettingContainer::Direction::Horizontal;
}

void ColorMapSettingToolBarWidget::applySetting()
{
	if (m_setting == nullptr) {return;}
	auto s = dynamic_cast<ColorMapSettingContainer*> (m_setting);

	ui->visibleCheckBox->blockSignals(true);
	ui->horizontalRadioButton->blockSignals(true);
	ui->verticalRadioButton->blockSignals(true);

	ui->visibleCheckBox->setChecked(s->legend.visible);
	ui->horizontalRadioButton->setEnabled(s->legend.visible);
	ui->verticalRadioButton->setEnabled(s->legend.visible);
	if (s->legend.direction == ColorMapLegendSettingContainer::Direction::Horizontal) {
		ui->horizontalRadioButton->setChecked(true);
	}	else if (s->legend.direction == ColorMapLegendSettingContainer::Direction::Vertical) {
		ui->verticalRadioButton->setChecked(true);
	}

	ui->visibleCheckBox->blockSignals(false);
	ui->horizontalRadioButton->blockSignals(false);
	ui->verticalRadioButton->blockSignals(false);
}
