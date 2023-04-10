#include "preprocessorgridcrosssectionwindow2_gridattributedisplaysettingcontainer.h"
#include "preprocessorgridcrosssectionwindow2_gridattributedisplaysettingeditdialog.h"
#include "preprocessorgridcrosssectionwindow2_impl.h"
#include "ui_preprocessorgridcrosssectionwindow2_gridattributedisplaysettingeditdialog.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>

#include <misc/iricundostack.h>
#include <misc/valuemodifycommandt.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::GridAttributeDisplaySettingEditDialog(GridAttributeDisplaySettingContainer* setting, PreProcessorGridCrosssectionWindow2* window) :
	QDialog {window},
	m_applied {false},
	m_colorMapEditWidget {setting->attribute()->createColorMapSettingEditWidget(this)},
	m_setting {setting},
	m_window {window},
	ui(new Ui::PreProcessorGridCrosssectionWindow2_GridAttributeDisplaySettingEditDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &GridAttributeDisplaySettingEditDialog::handleButtonClick);

	ui->colorMapWidget->setWidget(m_colorMapEditWidget);
	setSetting(*setting);
}

PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::~GridAttributeDisplaySettingEditDialog()
{
	delete ui;
}

QUndoCommand* PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::createModifyCommand(bool apply)
{
	auto ret = new MergeSupportedListCommand(iRIC::generateCommandId("PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::ModifyCommand"), apply);

	ret->addCommand(new ValueModifyCommmand<GridAttributeDisplaySettingContainer>(iRIC::generateCommandId("ModifySetting"), true, setting(), m_setting));
	ret->addCommand(m_colorMapEditWidget->createModifyCommand());

	return ret;
}

void PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::accept()
{
	m_window->pushUpdateCommand(createModifyCommand(false));
	QDialog::accept();
}

void PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingContainer PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::setting() const
{
	GridAttributeDisplaySettingContainer ret(m_setting->attribute());

	ret.visible = ui->visibleCheckBox->isChecked();

	if (ui->elevationChartRadioButton->isChecked()) {
		ret.mode = GridAttributeDisplaySettingContainer::Mode::AsElevation;
	} else if (ui->independentChartRadioButton->isChecked()) {
		ret.mode = GridAttributeDisplaySettingContainer::Mode::Chart;
	}	 else {
		ret.mode = GridAttributeDisplaySettingContainer::Mode::ColorMap;
	}

	ret.elevationChartLineWidth = ui->elevationChartLineWidthSpinBox->value();
	ret.elevationChartLineColor = ui->elevationChartLineColorWidget->color();
	ret.elevationChartShowPoint = ui->elevationShowPointsCheckBox->isChecked();
	ret.elevationChartPointSize = ui->elevationChartPointSizeSpinBox->value();
	ret.elevationChartShowIndex = ui->elevationShowIndicesCheckBox->isChecked();
	ret.elevationChartIndexFont = ui->elevationChartIndexFontWidget->font();
	ret.elevationChartIndexColor = ui->elevationChartIndexFontColorWidget->color();
	ret.elevationChartIndexSkipRate = ui->elevationChartIndexSkipRateSpinBox->value();

	ret.independentChartHeight = ui->independentChartHeightSpinBox->value();
	ret.independentChartLineWidth = ui->independentChartLineWidthSpinBox->value();
	ret.independentChartLineColor = ui->independentChartLineColorWidget->color();
	ret.independentChartTitleVisible = ui->independentChartTitleVisibleCheckBox->isChecked();
	ret.independentChartTitleFont = ui->independentChartTitleFontWidget->font();
	ret.independentChartAxisFont = ui->independentChartAxisFontWidget->font();
	ret.independentChartAxisColor = ui->independentChartAxisFontColorWidget->color();

	ret.colorMapHeight = ui->colorMapHeightSpinBox->value();
	ret.colorMapTitleVisible = ui->colorMapTitleVisibleCheckBox->isChecked();
	ret.colorMapTitleFont = ui->colorMapTitleFontWidget->font();
	ret.colorMapTitleColor = ui->colorMapTitleFontColorWidget->color();
	ret.usePreColor = ui->usePreCheckBox->isChecked();
	ret.colorMapSetting = m_setting->colorMapSetting;

	return ret;
}

void PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::setSetting(const GridAttributeDisplaySettingContainer& setting)
{
	ui->visibleCheckBox->setChecked(setting.visible);

	if (setting.mode == GridAttributeDisplaySettingContainer::Mode::AsElevation) {
		ui->elevationChartRadioButton->setChecked(true);
	} else if (setting.mode == GridAttributeDisplaySettingContainer::Mode::Chart) {
		ui->independentChartRadioButton->setChecked(true);
	} else if (setting.mode == GridAttributeDisplaySettingContainer::Mode::ColorMap) {
		ui->colorMapRadioButton->setChecked(true);
	}

	ui->elevationChartLineWidthSpinBox->setValue(setting.elevationChartLineWidth);
	ui->elevationChartLineColorWidget->setColor(setting.elevationChartLineColor);
	ui->elevationShowPointsCheckBox->setChecked(setting.elevationChartShowPoint);
	ui->elevationChartPointSizeSpinBox->setValue(setting.elevationChartPointSize);
	ui->elevationShowIndicesCheckBox->setChecked(setting.elevationChartShowIndex);
	ui->elevationChartIndexFontWidget->setFont(setting.elevationChartIndexFont);
	ui->elevationChartIndexFontColorWidget->setColor(setting.elevationChartIndexColor);
	ui->elevationChartIndexSkipRateSpinBox->setValue(setting.elevationChartIndexSkipRate);

	ui->independentChartHeightSpinBox->setValue(setting.independentChartHeight);
	ui->independentChartLineWidthSpinBox->setValue(setting.independentChartLineWidth);
	ui->independentChartLineColorWidget->setColor(setting.independentChartLineColor);
	ui->independentChartTitleVisibleCheckBox->setChecked(setting.independentChartTitleVisible);
	ui->independentChartTitleFontWidget->setFont(setting.independentChartTitleFont);
	ui->independentChartAxisFontWidget->setFont(setting.independentChartAxisFont);
	ui->independentChartAxisFontColorWidget->setColor(setting.independentChartAxisColor);

	ui->colorMapHeightSpinBox->setValue(setting.colorMapHeight);
	ui->colorMapTitleVisibleCheckBox->setChecked(setting.colorMapTitleVisible);
	ui->colorMapTitleFontWidget->setFont(setting.colorMapTitleFont);
	ui->colorMapTitleFontColorWidget->setColor(setting.colorMapTitleColor);
	ui->usePreCheckBox->setChecked(setting.usePreColor);

	if (setting.usePreColor) {
		auto cm = m_window->impl->preColorMapSetting(setting.attribute()->name());
		setting.colorMapSetting->copy(*cm);
	}
	m_colorMapEditWidget->setSetting(setting.colorMapSetting);
}

void PreProcessorGridCrosssectionWindow2::GridAttributeDisplaySettingEditDialog::apply()
{
	m_applied = true;
	m_window->pushUpdateCommand(createModifyCommand(true));
}
