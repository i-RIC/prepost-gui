#include "arrowssettingcontainer.h"
#include "arrowssettingtoolbarwidget.h"
#include "../scalarstocolors/colormapsettingcontainer.h"
#include "ui_arrowssettingtoolbarwidget.h"

#include <misc/stringtool.h>

#include <map>

ArrowsSettingToolBarWidget::ArrowsSettingToolBarWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ArrowsSettingToolBarWidget)
{
	ui->setupUi(this);

	connect<void (QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &ArrowsSettingToolBarWidget::handleValueChange);

	connect<void (QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &ArrowsSettingToolBarWidget::updated);
	connect(ui->colorWidget, &ColorEditWidget::colorChanged, this, &ArrowsSettingToolBarWidget::updated);
	connect(ui->lengthLegendCheckBox, &QCheckBox::toggled, this, &ArrowsSettingToolBarWidget::updated);
	connect(ui->colorMapWidget, &ColorMapSettingToolBarWidget::updated, this, &ArrowsSettingToolBarWidget::updated);
}

ArrowsSettingToolBarWidget::~ArrowsSettingToolBarWidget()
{
	delete ui;
}

void ArrowsSettingToolBarWidget::setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings)
{
	m_colorMapSettings = settings;

	auto cb = ui->valueComboBox;

	cb->blockSignals(true);
	cb->addItem(tr("(Custom Color)"));

	std::map<QString, std::string> captions;
	for (const auto& pair : settings) {
		captions.insert({pair.second->valueCaption, pair.first});
	}

	for (const auto& pair : captions) {
		cb->addItem(pair.first);
		m_colorMapNames.push_back(pair.second);
	}
	cb->blockSignals(false);
}

void ArrowsSettingToolBarWidget::setSetting(ArrowsSettingContainer* setting)
{
	m_setting = setting;
	connect(m_setting, &ArrowsSettingContainer::updated, this, &ArrowsSettingToolBarWidget::applySetting);

	applySetting();
}

void ArrowsSettingToolBarWidget::hideLegendCheckbox()
{
	ui->lengthLegendCheckBox->hide();
}

ArrowsSettingContainer ArrowsSettingToolBarWidget::modifiedSetting() const
{
	ArrowsSettingContainer ret = *m_setting;
	if (ui->valueComboBox->currentIndex() == 0) {
		ret.colorMode = ArrowsSettingContainer::ColorMode::Custom;
		ret.colorTarget = "";
	} else {
		ret.colorMode = ArrowsSettingContainer::ColorMode::ByScalar;
		ret.colorTarget = m_colorMapNames.at(ui->valueComboBox->currentIndex() - 1).c_str();
	}
	ret.customColor = ui->colorWidget->color();
	ret.legend.visible = ui->lengthLegendCheckBox->isChecked();

	return ret;
}

ColorMapSettingContainer ArrowsSettingToolBarWidget::modifiedColorMapSetting() const
{
	ColorMapSettingContainer ret;

	if (ui->valueComboBox->currentIndex() == 0) {return ret;}

	ret = *m_colorMapSettings.at(m_colorMapNames.at(ui->valueComboBox->currentIndex() - 1));
	ret.legend.visible = ui->colorMapWidget->visible();
	ret.legend.direction = ui->colorMapWidget->direction();

	return ret;
}

void ArrowsSettingToolBarWidget::handleValueChange(int index)
{
	ui->colorWidget->setEnabled(index == 0);
}

void ArrowsSettingToolBarWidget::applySetting()
{
	std::vector<QWidget*> widgets;

	widgets.push_back(ui->valueComboBox);
	widgets.push_back(ui->colorWidget);
	widgets.push_back(ui->lengthLegendCheckBox);
	widgets.push_back(ui->colorMapWidget);

	for (auto w : widgets) {
		w->blockSignals(true);
	}

	setDisabled(true);
	if (m_setting->target == "") {return;}

	setEnabled(true);

	if (m_setting->colorMode == ArrowsSettingContainer::ColorMode::Custom || m_setting->colorTarget == "") {
		ui->valueComboBox->setCurrentIndex(0);
		ui->colorMapWidget->setDisabled(true);
	} else {
		auto it = std::find(m_colorMapNames.begin(), m_colorMapNames.end(), iRIC::toStr(m_setting->colorTarget));
		ui->valueComboBox->setCurrentIndex((it - m_colorMapNames.begin()) + 1);
		ui->colorMapWidget->setEnabled(true);

		auto cm = m_colorMapSettings.at(iRIC::toStr(m_setting->colorTarget));
		ui->colorMapWidget->setSetting(cm);
	}
	ui->colorWidget->setColor(m_setting->customColor);
	ui->lengthLegendCheckBox->setChecked(m_setting->legend.visible);

	for (auto w : widgets) {
		w->blockSignals(false);
	}
}
