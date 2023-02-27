#include "arrowssettingcontainer.h"
#include "arrowscolorsettingeditsmallwidget.h"
#include "ui_arrowscolorsettingeditsmallwidget.h"

#include <guicore/scalarstocolors/colormapsettingeditdialog.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <misc/stringtool.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

ArrowsColorSettingEditSmallWidget::ArrowsColorSettingEditSmallWidget(QWidget *parent) :
	QWidget(parent),
	m_copyColorMapSettings {nullptr},
	m_mode {ColorMapMode::None},
	ui(new Ui::ArrowsColorSettingEditSmallWidget)
{
	ui->setupUi(this);
	connect<void(QComboBox::*)(int)>(ui->scalarComboBox, &QComboBox::currentIndexChanged, this, &ArrowsColorSettingEditSmallWidget::handleColorScalarChange);
	connect(ui->editButton, &QPushButton::clicked, [=](bool) {openColorMapEditDialog();});
}

ArrowsColorSettingEditSmallWidget::~ArrowsColorSettingEditSmallWidget()
{
	delete ui;
}

void ArrowsColorSettingEditSmallWidget::setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainer*>& settings)
{
	m_pointerColorMapSettings = settings;
	m_mode = ColorMapMode::Pointer;

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
		m_colorMapSetting = *(m_pointerColorMapSettings.at(m_colorMapNames.at(0)));
	} else {
		ui->customRadioButton->setChecked(true);
		ui->customRadioButton->setDisabled(true);
		ui->scalarRadioButton->setDisabled(true);
	}
}

void ArrowsColorSettingEditSmallWidget::setColorMapSettings(std::unordered_map<std::string, ColorMapSettingContainer>* settings)
{
	m_copyColorMapSettings = settings;
	m_mode = ColorMapMode::Copy;

	std::map<QString, std::string> sortedCaptions;
	for (const auto& pair : *settings) {
		sortedCaptions.insert({pair.second.valueCaption, pair.first});
	}

	ui->scalarComboBox->blockSignals(true);
	ui->scalarComboBox->clear();
	for (const auto& pair : sortedCaptions) {
		ui->scalarComboBox->addItem(pair.first);
		m_colorMapNames.push_back(pair.second);
	}
	ui->scalarComboBox->blockSignals(false);
	if (m_colorMapNames.size() == 0) {
		ui->customRadioButton->setChecked(true);
		ui->customRadioButton->setDisabled(true);
		ui->scalarRadioButton->setDisabled(true);
	}
}

ArrowsSettingContainer ArrowsColorSettingEditSmallWidget::setting() const
{
	ArrowsSettingContainer ret;
	updateSetting(&ret);
	return ret;
}

void ArrowsColorSettingEditSmallWidget::updateSetting(ArrowsSettingContainer* setting) const
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

void ArrowsColorSettingEditSmallWidget::setSetting(const ArrowsSettingContainer& setting)
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
			ui->scalarComboBox->setCurrentIndex(i);
		}
	}
}

QUndoCommand* ArrowsColorSettingEditSmallWidget::colorMapModifyCommand() const
{
	if (m_colorMapNames.size() == 0) {return nullptr;}
	if (m_mode != ColorMapMode::Pointer) {return nullptr;}

	auto cs = m_pointerColorMapSettings.at(m_colorMapNames.at(ui->scalarComboBox->currentIndex()));
	return new ValueModifyCommmand<ColorMapSettingContainer>(iRIC::generateCommandId("ArrowsColorSettingEditSmallWidget::colorMapModifyCommand"),
																													 true, m_colorMapSetting, cs);
}

void ArrowsColorSettingEditSmallWidget::handleColorScalarChange(int index)
{
	if (m_mode != ColorMapMode::Pointer) {return;}

	auto name = m_colorMapNames.at(index);
	auto colorSetting = m_pointerColorMapSettings.find(name)->second;

	m_colorMapSetting = *colorSetting;
}

void ArrowsColorSettingEditSmallWidget::openColorMapEditDialog()
{
	ColorMapSettingEditDialog dialog(this);
	ColorMapSettingEditWidget widget(&dialog);
	dialog.setWidget(&widget);
	dialog.disableApplyButton();

	ColorMapSettingContainer csForTitle;
	if (m_mode == ColorMapMode::Pointer) {
		widget.setConcreteSetting(m_colorMapSetting);
		csForTitle = m_colorMapSetting;
	} else if (m_mode == ColorMapMode::Copy) {
		widget.setConcreteSetting(m_copyColorMapSettings->at(m_colorMapNames.at(ui->scalarComboBox->currentIndex())));
		csForTitle = m_copyColorMapSettings->at(m_colorMapNames.at(ui->scalarComboBox->currentIndex()));
	}

	dialog.setWindowTitle(tr("Colormap Setting (%1)").arg(csForTitle.valueCaption));
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	if (m_mode == ColorMapMode::Pointer) {
		m_colorMapSetting = widget.concreteSetting();
	} else if (m_mode == ColorMapMode::Copy) {
		m_copyColorMapSettings->at(m_colorMapNames.at(ui->scalarComboBox->currentIndex())) = widget.concreteSetting();
	}
}
