#include "geodatanetcdf_displaysettingwidget.h"
#include "ui_geodatanetcdf_displaysettingwidget.h"

#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GeoDataNetcdf::DisplaySettingWidget::DisplaySettingWidget(QWidget *parent) :
	ModifyCommandWidget {parent},
	m_displaySetting {nullptr},
	m_colorMapWidget {nullptr},
	ui(new Ui::GeoDataNetcdf_DisplaySettingWidget)
{
	ui->setupUi(this);
	connect(ui->byValueRadioButton, &QRadioButton::toggled, this, &DisplaySettingWidget::handleColorByValueToggle);
}

GeoDataNetcdf::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete m_colorMapWidget;
	delete ui;
}

QUndoCommand* GeoDataNetcdf::DisplaySettingWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataNetcdf::DisplaySettingWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<DisplaySetting>(iRIC::generateCommandId("DisplaySetting"), true, setting(), m_displaySetting));
	if (m_colorMapWidget != nullptr) {
		command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	}
	return command;
}

GeoDataNetcdf::DisplaySetting GeoDataNetcdf::DisplaySettingWidget::setting() const
{
	DisplaySetting setting;
	if (ui->arbitraryRadioButton->isChecked()) {
		setting.mapping = DisplaySetting::Mapping::Arbitrary;
	} else {
		setting.mapping = DisplaySetting::Mapping::Value;
	}
	setting.color = ui->colorEditWidget->color();
	setting.opacity = ui->transparencyWidget->opacityPercent();

	return setting;
}

void GeoDataNetcdf::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	if (setting.mapping == DisplaySetting::Mapping::Arbitrary) {
		ui->arbitraryRadioButton->setChecked(true);
	} else if (setting.mapping == DisplaySetting::Mapping::Value) {
		ui->byValueRadioButton->setChecked(true);
	}
	ui->colorEditWidget->setColor(setting.color);
	ui->transparencyWidget->setOpacityPercent(setting.opacity);
}

void GeoDataNetcdf::DisplaySettingWidget::setSetting(DisplaySetting* setting)
{
	m_displaySetting = setting;
	setSetting(*setting);
}

void GeoDataNetcdf::DisplaySettingWidget::setColorMapWidget(ModifyCommandWidget* widget)
{
	m_colorMapWidget = widget;
	ui->colorMapWidget->setWidget(widget);
}

void GeoDataNetcdf::DisplaySettingWidget::setIsReferenceInformation(bool isReference)
{
	if (! isReference) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->arbitraryRadioButton->setDisabled(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataNetcdf::DisplaySettingWidget::handleColorByValueToggle(bool toggled)
{
	m_colorMapWidget->setEnabled(toggled);
}
