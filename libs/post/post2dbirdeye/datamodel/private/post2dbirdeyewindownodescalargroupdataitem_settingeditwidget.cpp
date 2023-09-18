#include "../post2dbirdeyewindowgridtypedataitem.h"
#include "../post2dbirdeyewindowzonedataitem.h"
#include "../post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "post2dbirdeyewindownodescalargroupdataitem_impl.h"
#include "post2dbirdeyewindownodescalargroupdataitem_setting.h"
#include "post2dbirdeyewindownodescalargroupdataitem_settingeditwidget.h"
#include "post2dbirdeyewindownodescalargroupdataitem_updatesettingcommand.h"
#include "ui_post2dbirdeyewindownodescalargroupdataitem_settingeditwidget.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/grid/v4grid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::SettingEditWidget(Post2dBirdEyeWindowNodeScalarGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_colorMapEditWidget {nullptr},
	m_item {item},
	ui(new Ui::Post2dBirdEyeWindowNodeScalarGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->nodeScalarRadioButton, &QRadioButton::clicked, this, &SettingEditWidget::handleNodeRadioButtonClick);
	connect(ui->cellScalarRadioButton, &QRadioButton::clicked, this, &SettingEditWidget::handleCellRadioButtonClick);
	connect<void (QComboBox::*)(int)>(ui->nodeScalarComboBox, &QComboBox::currentIndexChanged, this, &SettingEditWidget::handleNodeScalarChange);
	connect<void (QComboBox::*)(int)>(ui->cellScalarComboBox, &QComboBox::currentIndexChanged, this, &SettingEditWidget::handleCellScalarChange);

	auto zoneItem = m_item->topDataItem()->zoneDataItem();
	auto gridType = zoneItem->gridTypeDataItem()->gridType();
	auto data = zoneItem->v4DataContainer()->gridData()->grid()->vtkData()->data();

	std::map<QString, std::string> nodeCaptionMap;
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data->GetPointData())) {
		auto caption = gridType->outputCaption(name);
		nodeCaptionMap.insert({caption, name});
	}
	ui->nodeScalarComboBox->blockSignals(true);
	for (const auto& pair : nodeCaptionMap) {
		ui->nodeScalarComboBox->addItem(pair.first);
		m_nodeValueNames.push_back(pair.second);
	}
	ui->nodeScalarComboBox->blockSignals(false);
	if (m_nodeValueNames.size() == 0) {ui->nodeScalarRadioButton->setDisabled(true);}

	std::map<QString, std::string> cellCaptionMap;
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data->GetCellData())) {
		auto caption = gridType->outputCaption(name);
		cellCaptionMap.insert({caption, name});
	}
	ui->cellScalarComboBox->blockSignals(true);
	for (const auto& pair : cellCaptionMap) {
		ui->cellScalarComboBox->addItem(pair.first);
		m_cellValueNames.push_back(pair.second);
	}
	ui->cellScalarComboBox->blockSignals(false);
	if (m_cellValueNames.size() == 0) {ui->cellScalarRadioButton->setDisabled(true);}

	auto container = zoneItem->v4DataContainer();
	if (! container->gridData()->ibcExists(v4SolutionGrid::Position::Node)) {
		ui->rangeWidget->disableActive();
	}

	auto sgrid = vtkStructuredGrid::SafeDownCast(data);
	if (sgrid == nullptr) {
		ui->rangeWidget->hideCustom();
	} else {
		int dims[3];
		sgrid->GetDimensions(dims);
		ui->rangeWidget->setDimensions(dims[0], dims[1]);
	}

	setSetting(item->impl->m_setting);
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto s = setting();
	auto com1 = new ValueModifyCommmand<Setting>(iRIC::generateCommandId("Setting"), apply, s, &m_item->impl->m_setting);
	QUndoCommand* com2 = nullptr;
	if (s.colorMode != Setting::ColorMode::Custom) {
		com2 = m_colorMapEditWidget->createModifyCommand();
	}

	return new UpdateSettingCommand(apply, com1, com2);
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::setting() const
{
	Setting setting;
	setting.regionSetting = ui->rangeWidget->setting();

	if (ui->specificRadioButton->isChecked()) {
		setting.colorMode = Setting::ColorMode::Custom;
	} else if (ui->nodeScalarRadioButton->isChecked()) {
		setting.colorMode = Setting::ColorMode::ByNodeScalar;
		setting.colorTarget = m_nodeValueNames.at(ui->nodeScalarComboBox->currentIndex()).c_str();
	} else if (ui->cellScalarRadioButton->isChecked()) {
		setting.colorMode = Setting::ColorMode::ByCellScalar;
		setting.colorTarget = m_cellValueNames.at(ui->cellScalarComboBox->currentIndex()).c_str();
	}
	setting.customColor = ui->colorEditWidget->color();
	setting.contourSetting = ui->contourWidget->setting();
	setting.opacity = ui->opacityWidget->opacity();

	return setting;
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->rangeWidget->setSetting(setting.regionSetting);
	if (setting.colorMode == Setting::ColorMode::Custom) {
		ui->specificRadioButton->setChecked(true);
	} else if (setting.colorMode == Setting::ColorMode::ByNodeScalar) {
		ui->nodeScalarRadioButton->setChecked(true);

		auto it = std::find(m_nodeValueNames.begin(), m_nodeValueNames.end(), iRIC::toStr(setting.colorTarget));
		if (it == m_nodeValueNames.end()) {it = m_nodeValueNames.begin();}

		auto comboBox = ui->nodeScalarComboBox;
		comboBox->blockSignals(true);
		comboBox->setCurrentIndex(it - m_nodeValueNames.begin());
		handleNodeScalarChange(it - m_nodeValueNames.begin());
		comboBox->blockSignals(false);
	} else if (setting.colorMode == Setting::ColorMode::ByCellScalar) {
		ui->cellScalarRadioButton->setChecked(true);

		auto it = std::find(m_cellValueNames.begin(), m_cellValueNames.end(), iRIC::toStr(setting.colorTarget));
		if (it == m_cellValueNames.end()) {it = m_cellValueNames.begin();}

		auto comboBox = ui->cellScalarComboBox;
		comboBox->blockSignals(true);
		comboBox->setCurrentIndex(it - m_cellValueNames.begin());
		handleNodeScalarChange(it - m_cellValueNames.begin());
		comboBox->blockSignals(false);
	}
	ui->colorEditWidget->setColor(setting.customColor);
	ui->contourWidget->setSetting(setting.contourSetting);
	ui->opacityWidget->setOpacity(setting.opacity);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::setSetting(Setting* setting)
{
	setSetting(*setting);
	m_setting = setting;
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::handleNodeRadioButtonClick()
{
	handleNodeScalarChange(ui->nodeScalarComboBox->currentIndex());
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::handleCellRadioButtonClick()
{
	handleCellScalarChange(ui->cellScalarComboBox->currentIndex());
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::handleNodeScalarChange(int index)
{
	auto name = m_nodeValueNames.at(index);
	auto cs = m_item->impl->m_colorMapSettings.find(name)->second;
	if (m_colorMapEditWidget != nullptr) {
		auto oldSetting = m_colorMapEditWidget->setting();
		cs->legendSetting()->copyOtherThanTitle(*oldSetting->legendSetting());
	}

	auto output = m_item->topDataItem()->zoneDataItem()->v4DataContainer()->gridType()->output(name);
	m_colorMapEditWidget = output->createColorMapSettingEditWidget(this);
	m_colorMapEditWidget->setSetting(cs);
	auto widget = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapEditWidget, this);
	ui->colorMapWidget->setWidget(widget);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::handleCellScalarChange(int index)
{
	auto name = m_cellValueNames.at(index);
	auto cs = m_item->impl->m_colorMapSettings.find(name)->second;
	if (m_colorMapEditWidget != nullptr) {
		auto oldSetting = m_colorMapEditWidget->setting();
		cs->legendSetting()->copyOtherThanTitle(*oldSetting->legendSetting());
	}

	auto output = m_item->topDataItem()->zoneDataItem()->v4DataContainer()->gridType()->output(name);
	m_colorMapEditWidget = output->createColorMapSettingEditWidget(this);
	m_colorMapEditWidget->setSetting(cs);
	auto widget = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapEditWidget, this);
	ui->colorMapWidget->setWidget(widget);
}
