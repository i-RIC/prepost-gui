#include "geodatariversurvey_displaysettingwidget.h"
#include "ui_geodatariversurvey_displaysettingwidget.h"

#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GeoDataRiverSurvey::DisplaySettingWidget::DisplaySettingWidget(QWidget *parent) :
	ModifyCommandWidget {parent},
	m_displaySetting {nullptr},
	m_colorMapWidget {nullptr},
	ui(new Ui::GeoDataRiverSurvey_DisplaySettingWidget)
{
	ui->setupUi(this);
}

GeoDataRiverSurvey::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete m_colorMapWidget;
	delete ui;
}

QUndoCommand* GeoDataRiverSurvey::DisplaySettingWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataRiverSurvey::DisplaySettingWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<DisplaySetting>(iRIC::generateCommandId("DisplaySetting"), true, setting(), m_displaySetting));
	if (m_colorMapWidget != nullptr) {
		command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	}
	return command;
}

GeoDataRiverSurvey::DisplaySetting GeoDataRiverSurvey::DisplaySettingWidget::setting() const
{
	DisplaySetting setting;

	setting.showBackground = ui->bgVisibleCheckBox->isChecked();
	setting.showLines = ui->clVisibleCheckBox->isChecked();
	setting.opacity = ui->transparencyWidget->opacity();
	setting.crosssectionLinesScale = ui->zScaleSpinBox->value();
	setting.crosssectionLinesColor = ui->colorWidget->color();
	setting.showNames = ui->nameVisibleCheckBox->isChecked();
	setting.namesTextSetting = ui->nameFontWidget->setting();

	return setting;
}

void GeoDataRiverSurvey::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	ui->bgVisibleCheckBox->setChecked(setting.showBackground);
	ui->clVisibleCheckBox->setChecked(setting.showLines);
	ui->transparencyWidget->setOpacity(setting.opacity);
	ui->zScaleSpinBox->setValue(setting.crosssectionLinesScale);
	ui->colorWidget->setColor(setting.crosssectionLinesColor);
	ui->nameVisibleCheckBox->setChecked(setting.showNames);
	ui->nameFontWidget->setSetting(setting.namesTextSetting);
}

void GeoDataRiverSurvey::DisplaySettingWidget::setSetting(DisplaySetting* setting)
{
	m_displaySetting = setting;
	setSetting(*setting);
}

void GeoDataRiverSurvey::DisplaySettingWidget::setColorMapWidget(ModifyCommandWidget* widget)
{
	m_colorMapWidget = widget;
	ui->colorMapWidget->setWidget(widget);
}
