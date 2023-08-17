#include "polydatasetting.h"
#include "polydatasettingeditwidget.h"
#include "ui_polydatasettingeditwidget.h"
#include "private/polydatasettingeditwidget_modifycommand.h"

#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

PolyDataSettingEditWidget::PolyDataSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget {parent},
	m_setting {nullptr},
	m_colorMapWidget {nullptr},
	ui(new Ui::PolyDataSettingEditWidget)
{
	ui->setupUi(this);
	connect<void(QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &PolyDataSettingEditWidget::colorTargetChanged);
}

PolyDataSettingEditWidget::~PolyDataSettingEditWidget()
{
	delete ui;
}

void PolyDataSettingEditWidget::setSetting(PolyDataSetting* setting)
{
	m_setting = setting;
	setSetting(*setting);
}

void PolyDataSettingEditWidget::setGridType(SolverDefinitionGridType* gridType)
{
	m_gridType = gridType;
}

void PolyDataSettingEditWidget::setValueNames(const std::unordered_map<std::string, QString>& names)
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

void PolyDataSettingEditWidget::setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings)
{
	m_colorMapSettings = settings;
}

QUndoCommand* PolyDataSettingEditWidget::createModifyCommand(bool apply)
{
	auto settingCommand = new ValueModifyCommmand<PolyDataSetting>(iRIC::generateCommandId("Edit setting"), apply, setting(), m_setting);
	QUndoCommand* colorMapCommand = nullptr;
	if (ui->valueRadioButton->isChecked()) {
		colorMapCommand = m_colorMapWidget->createModifyCommand();
	}
	return new ModifyCommand(apply, settingCommand, colorMapCommand);
}

void PolyDataSettingEditWidget::colorTargetChanged(int index)
{
	auto colorTarget = m_colorTargets.at(index);
	auto cs = m_colorMapSettings.at(colorTarget);

	if (m_colorMapWidget != nullptr) {
		cs->legendSetting()->copyOtherThanTitle(*m_colorMapWidget->setting()->legendSetting());
	}

	auto output = m_gridType->output(colorTarget);
	m_colorMapWidget = output->createColorMapSettingEditWidget(this);
	m_colorMapWidget->setSetting(cs);
	auto widget = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapWidget, this);
	ui->colorMapWidget->setWidget(widget);
}

PolyDataSetting PolyDataSettingEditWidget::setting() const
{
	PolyDataSetting ret;

	if (ui->customColorRadioButton->isChecked()) {
		ret.mapping = PolyDataSetting::Mapping::Arbitrary;
	} else {
		ret.mapping = PolyDataSetting::Mapping::Value;
	}
	ret.color = ui->customColorWidget->color();
	if (m_colorTargets.size() > 0) {
		ret.value = colorTarget().c_str();
	}
	ret.opacity = ui->transparencyWidget->opacity();
	ret.lineWidth = ui->lineWidthSpinBox->value();

	return ret;
}

void PolyDataSettingEditWidget::setSetting(const PolyDataSetting& setting)
{
	if (setting.mapping == PolyDataSetting::Mapping::Arbitrary || m_colorTargets.size() == 0) {
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

	ui->transparencyWidget->setOpacity(setting.opacity);
	ui->lineWidthSpinBox->setValue(setting.lineWidth);
}

std::string PolyDataSettingEditWidget::colorTarget() const
{
	return m_colorTargets.at(ui->valueComboBox->currentIndex());
}

