#include "abstractcrosssectionwindow_gridattributedisplaysettingcontainer.h"
#include "abstractcrosssectionwindow_gridattributedisplaysettingeditdialog.h"
#include "abstractcrosssectionwindow_impl.h"
#include "ui_abstractcrosssectionwindow_gridattributedisplaysettingeditdialog.h"

#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>

#include <misc/iricundostack.h>
#include <misc/valuemodifycommandt.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::GridAttributeDisplaySettingEditDialog(GridAttributeDisplaySettingContainer* setting, AbstractCrosssectionWindow* window) :
	QDialog {window},
	m_applied {false},
	m_colorMapEditWidget {setting->attributeDataProvider()->createColorMapSettingEditWidget(setting->attributeName(), this)},
	m_setting {setting},
	m_window {window},
	ui(new Ui::AbstractCrosssectionWindow_GridAttributeDisplaySettingEditDialog)
{
	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &GridAttributeDisplaySettingEditDialog::handleButtonClick);

	auto cmEditWidget = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapEditWidget, this);

	ui->colorMapWidget->setWidget(cmEditWidget);
	setSetting(*setting);
}

AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::~GridAttributeDisplaySettingEditDialog()
{
	delete ui;
}

QUndoCommand* AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::createModifyCommand(bool apply)
{
	auto ret = new MergeSupportedListCommand(iRIC::generateCommandId("AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::ModifyCommand"), apply);

	ret->addCommand(new ValueModifyCommmand<GridAttributeDisplaySettingContainer>(iRIC::generateCommandId("ModifySetting"), true, setting(), m_setting));
	ret->addCommand(m_colorMapEditWidget->createModifyCommand());

	return ret;
}

void AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::accept()
{
	m_window->pushUpdateCommand(createModifyCommand(false));
	QDialog::accept();
}

void AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

AbstractCrosssectionWindow::GridAttributeDisplaySettingContainer AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::setting() const
{
	GridAttributeDisplaySettingContainer ret(*m_setting);

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

	ret.colorBarHeight = ui->colorMapHeightSpinBox->value();
	ret.colorBarTitleVisible = ui->colorMapTitleVisibleCheckBox->isChecked();
	ret.colorBarTitleFont = ui->colorMapTitleFontWidget->font();
	ret.colorBarTitleColor = ui->colorMapTitleFontColorWidget->color();
	ret.usePreColor = ui->usePreCheckBox->isChecked();
	ret.colorMapSetting = m_setting->colorMapSetting;

	return ret;
}

void AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::setSetting(const GridAttributeDisplaySettingContainer& setting)
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

	ui->colorMapHeightSpinBox->setValue(setting.colorBarHeight);
	ui->colorMapTitleVisibleCheckBox->setChecked(setting.colorBarTitleVisible);
	ui->colorMapTitleFontWidget->setFont(setting.colorBarTitleFont);
	ui->colorMapTitleFontColorWidget->setColor(setting.colorBarTitleColor);
	auto cm = m_window->preColorMapSetting(setting.attributeName());
	if (cm == nullptr) {
		ui->usePreCheckBox->hide();
		ui->usePreCheckBox->setChecked(false);
		m_colorMapEditWidget->setSetting(setting.colorMapSetting);
	} else {
		ui->usePreCheckBox->setChecked(setting.usePreColor);
		if (setting.usePreColor) {
			setting.colorMapSetting->copy(*cm);
		}
		m_colorMapEditWidget->setSetting(setting.colorMapSetting);
	}
}

void AbstractCrosssectionWindow::GridAttributeDisplaySettingEditDialog::apply()
{
	m_applied = true;
	m_window->pushUpdateCommand(createModifyCommand(true));
}
