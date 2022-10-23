#include "arrowssettingcontainer.h"
#include "arrowscolorsettingeditwidget.h"
#include "ui_arrowscolorsettingeditwidget.h"

#include <misc/stringtool.h>

ArrowsColorSettingEditWidget::ArrowsColorSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ArrowsColorSettingEditWidget)
{
	ui->setupUi(this);
	connect<void(QComboBox::*)(int)>(ui->scalarComboBox, &QComboBox::currentIndexChanged, this, &ArrowsColorSettingEditWidget::handleColorScalarChange);
	connect(ui->importButton, &QPushButton::clicked, [=](bool){ui->colorMapWidget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool){ui->colorMapWidget->exportSetting();});
}

ArrowsColorSettingEditWidget::~ArrowsColorSettingEditWidget()
{
	delete ui;
}

void ArrowsColorSettingEditWidget::setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings)
{
	m_colorMapSettings = settings;

	std::map<QString, std::string> sortedCaptions;
	for (const auto& pair : settings) {
		sortedCaptions.insert({pair.second->valueCaption, pair.first});
	}

	ui->scalarComboBox->blockSignals(true);
	ui->scalarComboBox->clear();
	for (const auto& pair : sortedCaptions) {
		ui->scalarComboBox->addItem(pair.first);
		m_colorMapNames.push_back(pair.second);
	}
	ui->scalarComboBox->blockSignals(false);
	if (m_colorMapNames.size() > 0) {
		ui->colorMapWidget->setSetting(m_colorMapSettings.at(m_colorMapNames.at(0)));
	} else {
		ui->customRadioButton->setChecked(true);
		ui->customRadioButton->setDisabled(true);
		ui->scalarRadioButton->setDisabled(true);
	}
}

ArrowsSettingContainer ArrowsColorSettingEditWidget::setting() const
{
	ArrowsSettingContainer ret;
	updateSetting(&ret);
	return ret;
}

void ArrowsColorSettingEditWidget::updateSetting(ArrowsSettingContainer* setting) const
{
	if (ui->customRadioButton->isChecked()) {
		setting->colorMode = ArrowsSettingContainer::ColorMode::Custom;
	} else {
		setting->colorMode = ArrowsSettingContainer::ColorMode::ByScalar;
	}
	setting->customColor = ui->colorEditWidget->color();
	if (m_colorMapNames.size() > 0) {
		setting->colorTarget = m_colorMapNames.at(ui->scalarComboBox->currentIndex()).c_str();
	}
}

void ArrowsColorSettingEditWidget::setSetting(const ArrowsSettingContainer& setting)
{
	if (setting.colorMode == ArrowsSettingContainer::ColorMode::Custom) {
		ui->customRadioButton->setChecked(true);
	} else if (setting.colorMode == ArrowsSettingContainer::ColorMode::ByScalar) {
		ui->scalarRadioButton->setChecked(true);
	}
	ui->colorEditWidget->setColor(setting.customColor);

	auto scalarStr = iRIC::toStr(setting.colorTarget);
	for (int i = 0; i < m_colorMapNames.size(); ++i) {
		if (m_colorMapNames.at(i) == scalarStr) {
			ui->scalarComboBox->blockSignals(true);
			ui->scalarComboBox->setCurrentIndex(i);
			ui->scalarComboBox->blockSignals(false);
			handleColorScalarChange(i);
		}
	}
}

QUndoCommand* ArrowsColorSettingEditWidget::colorMapModifyCommand() const
{
	if (m_colorMapNames.size() == 0) {
		return nullptr;
	}

	return ui->colorMapWidget->createModifyCommand();
}

void ArrowsColorSettingEditWidget::handleColorScalarChange(int index)
{
	auto name = m_colorMapNames.at(index);
	auto colorSetting = m_colorMapSettings.find(name)->second;

	ui->colorMapWidget->setSetting(colorSetting);
}
