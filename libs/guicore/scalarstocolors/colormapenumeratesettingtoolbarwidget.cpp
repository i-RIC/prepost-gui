#include "colormapenumeratesettingcontainer.h"
#include "colormapenumeratesettingtoolbarwidget.h"
#include "ui_colormapenumeratesettingtoolbarwidget.h"

ColorMapEnumerateSettingToolBarWidget::ColorMapEnumerateSettingToolBarWidget(QWidget *parent) :
	ColorMapSettingToolBarWidgetI(parent),
	ui(new Ui::ColorMapEnumerateSettingToolBarWidget)
{
	ui->setupUi(this);

	connect(ui->visibleCheckBox, &QCheckBox::toggled, [=](bool){emit updated();});
	connect(ui->horizontalRadioButton, &QCheckBox::clicked, [=](bool){emit updated();});
	connect(ui->verticalRadioButton, &QCheckBox::clicked, [=](bool){emit updated();});
}

ColorMapEnumerateSettingToolBarWidget::~ColorMapEnumerateSettingToolBarWidget()
{
	delete ui;
}

ColorMapSettingContainerI* ColorMapEnumerateSettingToolBarWidget::modifiedSetting()
{
	auto setting = new ColorMapEnumerateSettingContainer();
	*setting = *(dynamic_cast<ColorMapEnumerateSettingContainer*> (m_setting));

	setting->legend.visible = ui->visibleCheckBox->isChecked();

	if (ui->horizontalRadioButton->isChecked()) {
		setting->legend.direction = ColorMapEnumerateLegendSettingContainer::Direction::Horizontal;
	} else if (ui->verticalRadioButton->isChecked()) {
		setting->legend.direction = ColorMapEnumerateLegendSettingContainer::Direction::Vertical;
	}

	return setting;
}

void ColorMapEnumerateSettingToolBarWidget::applySetting()
{
	if (m_setting == nullptr) {return;}
	auto s = dynamic_cast<ColorMapEnumerateSettingContainer*> (m_setting);

	ui->visibleCheckBox->blockSignals(true);
	ui->horizontalRadioButton->blockSignals(true);
	ui->verticalRadioButton->blockSignals(true);

	ui->visibleCheckBox->setChecked(s->legend.visible);
	ui->horizontalRadioButton->setEnabled(s->legend.visible);
	ui->verticalRadioButton->setEnabled(s->legend.visible);
	if (s->legend.direction == ColorMapEnumerateLegendSettingContainer::Direction::Horizontal) {
		ui->horizontalRadioButton->setChecked(true);
	}	else if (s->legend.direction == ColorMapEnumerateLegendSettingContainer::Direction::Vertical) {
		ui->verticalRadioButton->setChecked(true);
	}

	ui->visibleCheckBox->blockSignals(false);
	ui->horizontalRadioButton->blockSignals(false);
	ui->verticalRadioButton->blockSignals(false);
}
