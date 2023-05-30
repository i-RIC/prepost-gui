#include "particledatasetting.h"
#include "particledatasettingeditwidget.h"
#include "ui_particledatasettingeditwidget.h"
#include "private/particledatasettingeditwidget_modifycommand.h"

#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

ParticleDataSettingEditWidget::ParticleDataSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget {parent},
	m_setting {nullptr},
	ui(new Ui::ParticleDataSettingEditWidget)
{
	ui->setupUi(this);
	connect<void(QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &ParticleDataSettingEditWidget::colorTargetChanged);

	m_colorMapWidget = new ColorMapSettingEditWidget(this);
	auto w = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapWidget, this);
	ui->colorMapWidget->setWidget(w);
}

ParticleDataSettingEditWidget::~ParticleDataSettingEditWidget()
{
	delete ui;
}

void ParticleDataSettingEditWidget::setSetting(ParticleDataSetting* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void ParticleDataSettingEditWidget::setValueNames(const std::unordered_map<std::string, QString>& names)
{
	ui->valueComboBox->blockSignals(true);

	std::map<QString, std::string> namesMap;
	for (const auto& pair : names) {
		namesMap.insert({pair.second, pair.first});
	}

	for (const auto& pair : namesMap) {
		ui->valueComboBox->addItem(pair.first);
		m_colorTargets.push_back(pair.second);
	}

	ui->valueComboBox->blockSignals(false);

	if (m_colorTargets.size() == 0) {
		ui->customColorRadioButton->setChecked(true);
		ui->customColorRadioButton->setDisabled(true);
		ui->valueRadioButton->setDisabled(true);
		ui->colorMapGroupBox->setDisabled(true);
	}
}

void ParticleDataSettingEditWidget::setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings)
{
	m_colorMapSettings = settings;
}

QUndoCommand* ParticleDataSettingEditWidget::createModifyCommand(bool apply)
{
	auto settingCommand = new ValueModifyCommmand<ParticleDataSetting>(iRIC::generateCommandId("Edit setting"), apply, setting(), m_setting);
	QUndoCommand* colorMapCommand = nullptr;
	if (ui->valueRadioButton->isChecked()) {
		colorMapCommand = m_colorMapWidget->createModifyCommand();
	}
	return new ModifyCommand(apply, settingCommand, colorMapCommand);
}

void ParticleDataSettingEditWidget::colorTargetChanged(int index)
{
	auto colorTarget = m_colorTargets.at(index);
	auto cs = m_colorMapSettings.at(colorTarget);

	m_colorMapWidget->setSetting(cs);
}

ParticleDataSetting ParticleDataSettingEditWidget::setting() const
{
	ParticleDataSetting ret;

	if (ui->customColorRadioButton->isChecked()) {
		ret.mapping = ParticleDataSetting::Mapping::Arbitrary;
	} else {
		ret.mapping = ParticleDataSetting::Mapping::Value;
	}
	ret.color = ui->customColorWidget->color();
	if (m_colorTargets.size() > 0) {
		ret.value = colorTarget().c_str();
	}
	ret.opacity = ui->transparencyWidget->opacityPercent();
	ret.particleSize = ui->pointSizeSpinBox->value();

	return ret;
}

void ParticleDataSettingEditWidget::setSetting(const ParticleDataSetting& setting)
{
	if (setting.mapping == ParticleDataSetting::Mapping::Arbitrary) {
		ui->customColorRadioButton->setChecked(true);
	} else {
		ui->valueRadioButton->setChecked(true);
	}
	ui->customColorWidget->setColor(setting.color);
	auto value = iRIC::toStr(setting.value);
	if (value == "" && m_colorTargets.size() > 0) {
		value = m_colorTargets.at(0);
	}

	bool found = false;
	for (int i = 0; i < static_cast<int>(m_colorTargets.size()); ++i) {
		if (value == m_colorTargets.at(i)) {
			ui->valueComboBox->setCurrentIndex(i);
			colorTargetChanged(i);
			found = true;
		}
	}
	if (! found && m_colorTargets.size() > 0) {
		ui->valueComboBox->setCurrentIndex(0);
		colorTargetChanged(0);
	}

	ui->transparencyWidget->setOpacityPercent(setting.opacity);
	ui->pointSizeSpinBox->setValue(setting.particleSize);
}

std::string ParticleDataSettingEditWidget::colorTarget() const
{
	return m_colorTargets.at(ui->valueComboBox->currentIndex());
}
