#include "polydatasetting.h"
#include "polydatasettingtoolbarwidget.h"
#include "ui_polydatasettingtoolbarwidget.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <misc/stringtool.h>

PolyDataSettingToolBarWidget::PolyDataSettingToolBarWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::PolyDataSettingToolBarWidget)
{
	ui->setupUi(this);

	connect<void (QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &PolyDataSettingToolBarWidget::handleValueChange);
	connect<void (QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &PolyDataSettingToolBarWidget::updated);
	connect(ui->colorWidget, &ColorEditWidget::colorChanged, this, &PolyDataSettingToolBarWidget::updated);
	connect(ui->colorMapWidget, &ColorMapSettingToolBarWidget::updated, this, &PolyDataSettingToolBarWidget::updated);
}

PolyDataSettingToolBarWidget::~PolyDataSettingToolBarWidget()
{
	delete ui;
}

void PolyDataSettingToolBarWidget::setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings)
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

void PolyDataSettingToolBarWidget::setSetting(PolyDataSetting* setting)
{
	m_setting = setting;
	connect(m_setting, &PolyDataSetting::updated, this, &PolyDataSettingToolBarWidget::applySetting);

	applySetting();
}

PolyDataSetting PolyDataSettingToolBarWidget::modifiedSetting() const
{
	PolyDataSetting ret = *m_setting;
	if (ui->valueComboBox->currentIndex() == 0) {
		ret.mapping = PolyDataSetting::Mapping::Arbitrary;
		ret.value = "";
	} else {
		ret.mapping = PolyDataSetting::Mapping::Value;
		ret.value = m_colorMapNames.at(ui->valueComboBox->currentIndex() - 1).c_str();
	}
	ret.color = ui->colorWidget->color();

	return ret;
}

ColorMapSettingContainerI* PolyDataSettingToolBarWidget::modifiedColorMapSetting() const
{
	if (ui->valueComboBox->currentIndex() == 0) {return nullptr;}

	ColorMapSettingContainerI* ret = m_colorMapSettings.at(m_colorMapNames.at(ui->valueComboBox->currentIndex() - 1))->copy();
	ret->legendSetting()->setVisible(ui->colorMapWidget->visible());
	ret->legendSetting()->setDirection(ui->colorMapWidget->direction());

	return ret;
}

void PolyDataSettingToolBarWidget::handleValueChange(int index)
{
	ui->colorWidget->setEnabled(index == 0);
	ui->colorMapWidget->setEnabled(index != 0);

	if (index > 0) {
		auto target = m_colorMapNames.at(index - 1);
		auto cm = m_colorMapSettings.at(target);
		ui->colorMapWidget->setSetting(cm);
	}
}

void PolyDataSettingToolBarWidget::applySetting()
{
	std::vector<QWidget*> widgets;

	widgets.push_back(ui->valueComboBox);
	widgets.push_back(ui->colorWidget);
	widgets.push_back(ui->colorMapWidget);

	for (auto w : widgets) {
		w->blockSignals(true);
	}

	if (m_setting->mapping == PolyDataSetting::Mapping::Arbitrary || m_setting->value == "") {
		ui->valueComboBox->setCurrentIndex(0);
		ui->colorMapWidget->setDisabled(true);
	} else {
		auto it = std::find(m_colorMapNames.begin(), m_colorMapNames.end(), iRIC::toStr(m_setting->value));
		ui->valueComboBox->setCurrentIndex((it - m_colorMapNames.begin()) + 1);
		ui->colorMapWidget->setEnabled(true);

		auto cm = m_colorMapSettings.at(iRIC::toStr(m_setting->value));
		ui->colorMapWidget->setSetting(cm);
	}
	ui->colorWidget->setColor(m_setting->color);

	for (auto w : widgets) {
		w->blockSignals(false);
	}
}
