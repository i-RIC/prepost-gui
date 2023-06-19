#include "geodatapointmap_displaysettingwidget.h"
#include "ui_geodatapointmap_displaysettingwidget.h"

#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GeoDataPointmap::DisplaySettingWidget::DisplaySettingWidget(QWidget *parent) :
	ModifyCommandWidget {parent},
	m_displaySetting {nullptr},
	m_colorMapWidget {nullptr},
	ui(new Ui::GeoDataPointmap_DisplaySettingWidget)
{
	ui->setupUi(this);
}

GeoDataPointmap::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete m_colorMapWidget;
	delete ui;
}

QUndoCommand* GeoDataPointmap::DisplaySettingWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataPointmap::DisplaySettingWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<DisplaySetting>(iRIC::generateCommandId("DisplaySetting"), true, setting(), m_displaySetting));
	if (m_colorMapWidget != nullptr) {
		command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	}
	return command;
}

GeoDataPointmap::DisplaySetting GeoDataPointmap::DisplaySettingWidget::setting() const
{
	DisplaySetting ret;
	if (ui->pointsRadioButton->isChecked()) {
		ret.tinRepresentation = DisplaySetting::TinRepresentation::Points;
	} else if (ui->wireframeRadioButton->isChecked()) {
		ret.tinRepresentation = DisplaySetting::TinRepresentation::Wireframe;
	} else if (ui->surfaceRadioButton->isChecked()) {
		ret.tinRepresentation = DisplaySetting::TinRepresentation::Surface;
	}
	ret.tinPointSize = ui->pointSizeSpinBox->value();
	ret.tinLineWidth = ui->lineWidthSpinBox->value();
	ret.tinOpacity = ui->tinOpacityWidget->opacity();

	ret.breakLineVisible = ui->showBreakLinesCheckBox->isChecked();
	ret.breakLineColor = ui->breakLineColorWidget->color();
	ret.breakLineWidth = ui->breakLineLineWidthSpinBox->value();

	ret.polygonsVisible = ui->showPolygonsCheckBox->isChecked();
	ret.polygonsLineWidth = ui->polygonLineWidthSpinBox->value();
	ret.polygonsOpacity = ui->polygonOpacityWidget->opacity();

	return ret;
}

void GeoDataPointmap::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	if (setting.tinRepresentation == DisplaySetting::TinRepresentation::Points) {
		ui->pointsRadioButton->setChecked(true);
	} else if (setting.tinRepresentation == DisplaySetting::TinRepresentation::Wireframe) {
		ui->wireframeRadioButton->setChecked(true);
	} else if (setting.tinRepresentation == DisplaySetting::TinRepresentation::Surface) {
		ui->surfaceRadioButton->setChecked(true);
	}
	ui->pointSizeSpinBox->setValue(setting.tinPointSize);
	ui->lineWidthSpinBox->setValue(setting.tinLineWidth);
	ui->tinOpacityWidget->setOpacity(setting.tinOpacity);

	ui->showBreakLinesCheckBox->setChecked(setting.breakLineVisible);
	ui->breakLineColorWidget->setColor(setting.breakLineColor);
	ui->breakLineLineWidthSpinBox->setValue(setting.breakLineWidth);

	ui->showPolygonsCheckBox->setChecked(setting.polygonsVisible);
	ui->polygonLineWidthSpinBox->setValue(setting.polygonsLineWidth);
	ui->polygonOpacityWidget->setOpacity(setting.polygonsOpacity);
}

void GeoDataPointmap::DisplaySettingWidget::setSetting(DisplaySetting* setting)
{
	m_displaySetting = setting;
	setSetting(*setting);
}

void GeoDataPointmap::DisplaySettingWidget::setColorMapWidget(ModifyCommandWidget* widget)
{
	m_colorMapWidget = widget;
	ui->colorMapWidget->setWidget(widget);
}
