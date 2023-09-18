#include "../../../../datamodel/preprocessorgridtypedataitem.h"
#include "../gridbirdeyewindowzonedataitem.h"
#include "../gridbirdeyewindownodescalargrouptopdataitem.h"
#include "gridbirdeyewindownodescalargroupdataitem_impl.h"
#include "gridbirdeyewindownodescalargroupdataitem_setting.h"
#include "gridbirdeyewindownodescalargroupdataitem_settingeditwidget.h"
#include "gridbirdeyewindownodescalargroupdataitem_updatesettingcommand.h"
#include "ui_gridbirdeyewindownodescalargroupdataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guibase/vtkpointsetextended/vtkpointsetextended.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::SettingEditWidget(GridBirdEyeWindowNodeScalarGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_colorMapEditWidget {nullptr},
	m_colorMapEditWidgetWithImportExport {nullptr},
	m_item {item},
	ui(new Ui::GridBirdEyeWindowNodeScalarGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->nodeScalarRadioButton, &QRadioButton::clicked, this, &SettingEditWidget::handleNodeRadioButtonClick);
	connect(ui->cellScalarRadioButton, &QRadioButton::clicked, this, &SettingEditWidget::handleCellRadioButtonClick);
	connect<void (QComboBox::*)(int)>(ui->nodeScalarComboBox, &QComboBox::currentIndexChanged, this, &SettingEditWidget::handleNodeScalarChange);
	connect<void (QComboBox::*)(int)>(ui->cellScalarComboBox, &QComboBox::currentIndexChanged, this, &SettingEditWidget::handleCellScalarChange);

	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (m_item->dataModel());
	auto gridType = model->gridTypeDataItem()->gridType();
	auto grid = m_item->topDataItem()->zoneDataItem()->grid()->grid()->vtkData()->data();

	std::map<QString, std::string> nodeCaptionMap;
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->GetPointData())) {
		nodeCaptionMap.insert({gridType->gridAttributeCaption(name), name});
	}
	ui->nodeScalarComboBox->blockSignals(true);
	for (const auto& pair : nodeCaptionMap) {
		ui->nodeScalarComboBox->addItem(pair.first);
		m_nodeValueNames.push_back(pair.second);
	}
	ui->nodeScalarComboBox->blockSignals(false);
	if (m_nodeValueNames.size() == 0) {ui->nodeScalarRadioButton->setDisabled(true);}

	std::map<QString, std::string> cellCaptionMap;
	for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(grid->GetCellData())) {
		cellCaptionMap.insert({gridType->gridAttributeCaption(name), name});
	}
	ui->cellScalarComboBox->blockSignals(true);
	for (const auto& pair : cellCaptionMap) {
		ui->cellScalarComboBox->addItem(pair.first);
		m_cellValueNames.push_back(pair.second);
	}
	ui->cellScalarComboBox->blockSignals(false);
	if (m_cellValueNames.size() == 0) {ui->cellScalarRadioButton->setDisabled(true);}

	ui->rangeWidget->disableActive();

	auto sgrid = vtkStructuredGrid::SafeDownCast(grid);
	if (sgrid == nullptr) {
		ui->rangeWidget->hideCustom();
	} else {
		int dims[3];
		sgrid->GetDimensions(dims);
		ui->rangeWidget->setDimensions(dims[0], dims[1]);
	}

	setSetting(item->impl->m_setting);
}

GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto s = setting();
	auto com1 = new ValueModifyCommmand<Setting>(iRIC::generateCommandId("Setting"), apply, s, &m_item->impl->m_setting);
	QUndoCommand* com2 = nullptr;
	if (s.colorMode != Setting::ColorMode::Custom) {
		com2 = m_colorMapEditWidget->createModifyCommand();
	}

	return new UpdateSettingCommand(apply, com1, com2);
}

GridBirdEyeWindowNodeScalarGroupDataItem::Setting GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::setting() const
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
	setting.opacity = ui->opacityWidget->opacity();

	return setting;
}

void GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::setSetting(const Setting& setting)
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
	ui->opacityWidget->setOpacity(setting.opacity);
}

void GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::setSetting(Setting* setting)
{
	setSetting(*setting);
	m_setting = setting;
}

void GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::handleNodeRadioButtonClick()
{
	handleNodeScalarChange(ui->nodeScalarComboBox->currentIndex());
}

void GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::handleCellRadioButtonClick()
{
	handleCellScalarChange(ui->cellScalarComboBox->currentIndex());
}

void GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::handleNodeScalarChange(int index)
{
	auto name = m_nodeValueNames.at(index);
	auto cs = m_item->impl->m_colorMapSettings.find(name)->second;
	if (m_colorMapEditWidget != nullptr) {
		auto oldSetting = m_colorMapEditWidget->setting();
		if (oldSetting != nullptr) {
			cs->legendSetting()->copyOtherThanTitle(*oldSetting->legendSetting());
		}
	}
	if (m_colorMapEditWidget != nullptr) {
		auto oldSetting = m_colorMapEditWidget->setting();
		if (oldSetting != nullptr) {
			cs->legendSetting()->copyOtherThanTitle(*oldSetting->legendSetting());
		}
	}
	auto oldWidget = m_colorMapEditWidgetWithImportExport;

	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (m_item->dataModel());
	m_colorMapEditWidget = model->gridTypeDataItem()->gridType()->createGridAttributeColorMapSettingEditWidget(name, this);
	m_colorMapEditWidgetWithImportExport = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapEditWidget, this);
	ui->colorMapWidget->setWidget(m_colorMapEditWidgetWithImportExport);

	m_colorMapEditWidget->setSetting(cs);

	delete oldWidget;
}

void GridBirdEyeWindowNodeScalarGroupDataItem::SettingEditWidget::handleCellScalarChange(int index)
{
	auto name = m_cellValueNames.at(index);
	auto cs = m_item->impl->m_colorMapSettings.find(name)->second;
	if (m_colorMapEditWidget != nullptr) {
		auto oldSetting = m_colorMapEditWidget->setting();
		if (oldSetting != nullptr) {
			cs->legendSetting()->copyOtherThanTitle(*oldSetting->legendSetting());
		}
	}
	auto oldWidget = m_colorMapEditWidgetWithImportExport;

	auto model = dynamic_cast<GridBirdEyeWindowDataModel*> (m_item->dataModel());
	m_colorMapEditWidget = model->gridTypeDataItem()->gridType()->createGridAttributeColorMapSettingEditWidget(name, this);
	m_colorMapEditWidgetWithImportExport = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapEditWidget, this);
	ui->colorMapWidget->setWidget(m_colorMapEditWidgetWithImportExport);

	m_colorMapEditWidget->setSetting(cs);

	delete oldWidget;
}
