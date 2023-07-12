#include "arrowssettingcontainer.h"
#include "arrowscolorsettingeditsmallwidget.h"
#include "ui_arrowscolorsettingeditsmallwidget.h"

#include <guicore/scalarstocolors/colormapsettingeditdialog.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingmodifycommand.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>


ArrowsColorSettingEditSmallWidget::ArrowsColorSettingEditSmallWidget(QWidget *parent) :
	QWidget(parent),
	m_colorMapSetting {nullptr},
	m_colorMapSettings {},
	m_gridType {nullptr},
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

void ArrowsColorSettingEditSmallWidget::setGridType(SolverDefinitionGridType* gridType)
{
	m_gridType = gridType;
}

void ArrowsColorSettingEditSmallWidget::setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings)
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
		handleColorScalarChange(0);
	} else {
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

	auto cs = m_colorMapSettings.at(m_colorMapNames.at(ui->scalarComboBox->currentIndex()));
	return new ColorMapSettingModifyCommand(m_colorMapSetting, cs);
}

void ArrowsColorSettingEditSmallWidget::handleColorScalarChange(int index)
{
	auto name = m_colorMapNames.at(index);
	auto origColorSetting = m_colorMapSettings.find(name)->second;
	auto newColorSetting = origColorSetting->copy();
	newColorSetting->valueCaption = origColorSetting->valueCaption;

	if (m_colorMapSetting != nullptr) {
		newColorSetting->legendSetting()->copyOtherThanTitle(*m_colorMapSetting->legendSetting());
	}
	delete m_colorMapSetting;
	m_colorMapSetting = newColorSetting;
}

void ArrowsColorSettingEditSmallWidget::openColorMapEditDialog()
{
	ColorMapSettingEditDialog dialog(this);

	auto name = m_colorMapNames.at(ui->scalarComboBox->currentIndex());
	auto output = m_gridType->output(name);
	ColorMapSettingEditWidgetI* widget = output->createColorMapSettingEditWidget(&dialog);
	dialog.setWidget(widget);
	dialog.disableApplyButton();

	widget->setSetting(m_colorMapSetting);
	dialog.setWindowTitle(tr("Colormap Setting (%1)").arg(m_colorMapSetting->valueCaption));
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto com = widget->createModifyCommand();
	com->redo();
	delete com;
}
