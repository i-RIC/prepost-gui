#include "geodatapolygongroup_displaysettingwidget.h"
#include "ui_geodatapolygongroup_displaysettingwidget.h"

#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

GeoDataPolygonGroup::DisplaySettingWidget::DisplaySettingWidget(QWidget *parent) :
	ModifyCommandWidget {parent},
	m_displaySetting {nullptr},
	m_colorMapWidget {nullptr},
	ui(new Ui::GeoDataPolygonGroup_DisplaySettingWidget)
{
	ui->setupUi(this);
	connect(ui->byValueRadioButton, &QRadioButton::toggled, this, &DisplaySettingWidget::handleColorByValueToggle);
}

GeoDataPolygonGroup::DisplaySettingWidget::~DisplaySettingWidget()
{
	delete m_colorMapWidget;
	delete ui;
}

QUndoCommand* GeoDataPolygonGroup::DisplaySettingWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("GeoDataPolygonGroup::DisplaySettingWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<DisplaySetting>(iRIC::generateCommandId("DisplaySetting"), true, setting(), m_displaySetting));
	if (m_colorMapWidget != nullptr) {
		command->addCommand(m_colorMapWidget->createModifyCommand(apply));
	}
	return command;
}

GeoDataPolygonGroup::DisplaySetting GeoDataPolygonGroup::DisplaySettingWidget::setting() const
{
	DisplaySetting setting;
	if (ui->arbitraryRadioButton->isChecked()) {
		setting.mapping = DisplaySetting::Mapping::Arbitrary;
	} else {
		setting.mapping = DisplaySetting::Mapping::Value;
	}
	setting.color = ui->colorEditWidget->color();
	setting.lineWidth = ui->lineWidthSpinBox->value();
	setting.opacity = ui->transparencyWidget->opacity();

	return setting;
}

void GeoDataPolygonGroup::DisplaySettingWidget::setSetting(const DisplaySetting& setting)
{
	if (setting.mapping == DisplaySetting::Mapping::Arbitrary) {
		ui->arbitraryRadioButton->setChecked(true);
	} else if (setting.mapping == DisplaySetting::Mapping::Value) {
		ui->byValueRadioButton->setChecked(true);
	}
	ui->colorEditWidget->setColor(setting.color);
	ui->lineWidthSpinBox->setValue(setting.lineWidth);
	ui->transparencyWidget->setOpacity(setting.opacity);
}

void GeoDataPolygonGroup::DisplaySettingWidget::setSetting(DisplaySetting* setting)
{
	m_displaySetting = setting;
	setSetting(*setting);
}

void GeoDataPolygonGroup::DisplaySettingWidget::setColorMapWidget(ModifyCommandWidget* widget)
{
	m_colorMapWidget = widget;
	ui->colorMapWidget->setWidget(widget);
}

void GeoDataPolygonGroup::DisplaySettingWidget::setIsReferenceInformation(bool isReference)
{
	if (! isReference) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->arbitraryRadioButton->setDisabled(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataPolygonGroup::DisplaySettingWidget::handleColorByValueToggle(bool toggled)
{
	if (m_colorMapWidget == nullptr) {return;}

	m_colorMapWidget->setEnabled(toggled);
}
