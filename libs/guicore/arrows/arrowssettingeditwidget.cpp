#include "../scalarstocolors/colormapsettingcontainer.h"
#include "../scalarstocolors/colormapsettingeditdialog.h"
#include "../scalarstocolors/colormapsettingeditwidget.h"
#include "arrowssettingcontainer.h"
#include "arrowssettingeditwidget.h"
#include "ui_arrowssettingeditwidget.h"
#include "private/arrowssettingeditwidget_modifycommand.h"

#include <misc/stringtool.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <QUndoCommand>

ArrowsSettingEditWidget::ArrowsSettingEditWidget(QWidget *parent) :
	QWidget(parent),
	m_valueNames {},
	m_setting {nullptr},
	ui(new Ui::ArrowsSettingEditWidget)
{
	ui->setupUi(this);
	connect<void(QComboBox::*)(const QString&)>(ui->valueComboBox, &QComboBox::currentTextChanged, this, &ArrowsSettingEditWidget::handleTargetChange);
}

ArrowsSettingEditWidget::~ArrowsSettingEditWidget()
{
	delete ui;
}

void ArrowsSettingEditWidget::setValueNames(const std::map<std::string, QString> names)
{
	ui->valueComboBox->clear();

	std::map<QString, std::string> sortedCaptions;
	for (const auto& pair : names) {
		sortedCaptions.insert({pair.second, pair.first});
	}

	for (const auto& pair : sortedCaptions) {
		ui->valueComboBox->addItem(pair.first);
		m_valueNames.push_back(pair.second);
	}
}

void ArrowsSettingEditWidget::setGridType(SolverDefinitionGridType* gridType)
{
	ui->colorSettingEditWidget->setGridType(gridType);
}

void ArrowsSettingEditWidget::setColorMapSettings(const std::unordered_map<std::string, ColorMapSettingContainerI*>& settings)
{
	ui->colorSettingEditWidget->setColorMapSettings(settings);
}

void ArrowsSettingEditWidget::setAdditionalSettingWidget(QWidget* widget)
{
	ui->additionalSettingWidget->setWidget(widget);
}

QTabWidget* ArrowsSettingEditWidget::tabWidget() const
{
	return ui->tabWidget;
}

ArrowsSettingContainer ArrowsSettingEditWidget::setting() const
{
	ArrowsSettingContainer ret;

	ret.target = m_valueNames.at(ui->valueComboBox->currentIndex()).c_str();
	ui->lengthSettingWidget->updateSetting(&ret);
	ui->colorSettingEditWidget->updateSetting(&ret);
	ui->arrowShapeWidget->updateSetting(&ret);

	ret.legend = ui->legendWidget->setting();

	return ret;
}

void ArrowsSettingEditWidget::setSetting(ArrowsSettingContainer* setting)
{
	m_setting = setting;

	auto targetStr = iRIC::toStr(setting->target);

	for (int i = 0; i < m_valueNames.size(); ++i) {
		if (m_valueNames.at(i) == targetStr) {
			ui->valueComboBox->setCurrentIndex(i);
		}
	}
	ui->lengthSettingWidget->setSetting(*setting);
	ui->colorSettingEditWidget->setSetting(*setting);
	ui->arrowShapeWidget->setSetting(*setting);

	ui->legendWidget->setSetting(setting->legend);

	if (targetStr == "") {
		ui->legendWidget->setTitle(ui->valueComboBox->currentText());
	}

	connect(&m_setting->legend.imageSetting, &ImageSettingContainer::updated, this, &ArrowsSettingEditWidget::updateImageSetting);
}

QUndoCommand* ArrowsSettingEditWidget::createModifyCommand() const
{
	auto arrowsCommand = new ValueModifyCommmand<ArrowsSettingContainer> (iRIC::generateCommandId("ModifyArrowSetting"), true, setting(), m_setting);
	auto colormapCommand = ui->colorSettingEditWidget->colorMapModifyCommand();

	return new ModifyCommand(arrowsCommand, colormapCommand);
}

void ArrowsSettingEditWidget::handleTargetChange(const QString& value)
{
	ui->legendWidget->setTitle(value);
}

void ArrowsSettingEditWidget::updateImageSetting()
{
	ui->legendWidget->setImageSetting(m_setting->legend.imageSetting);
}
