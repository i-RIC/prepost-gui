#include "gridbirdeyewindowgriddataitem_editdisplaysettingdialog.h"
#include "gridbirdeyewindowgriddataitem_updatedisplaysettingcommand.h"
#include "ui_gridbirdeyewindowgriddataitem_editdisplaysettingdialog.h"
#include "../../../datamodel/preprocessorgriddataitem.h"
#include "../../../datamodel/preprocessorgridtypedataitem.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

#include <misc/iricundostack.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::EditDisplaySettingDialog(GridBirdEyeWindowGridDataItem* item, QWidget *parent) :
	QDialog (parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::GridBirdEyeWindowGridDataItem_EditDisplaySettingDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);

	ui->setupUi(this);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &EditDisplaySettingDialog::handleButtonClick);
	connect<void(QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &EditDisplaySettingDialog::colorTargetChanged);

	m_gridType = dynamic_cast<PreProcessorGridTypeDataItem*> (m_item->gridDataItem()->parent()->parent())->gridType();

	setupWarpTargets();
	setupColorTargets();

	setSetting(item->m_setting);
}

GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::~EditDisplaySettingDialog()
{
	delete ui;
}

QUndoCommand* GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::createModifyCommand(bool apply)
{
	auto s = new ValueModifyCommmand<GridBirdEyeWindowSetting>(
				iRIC::generateCommandId("GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::editSetting"),
				apply, setting(), &m_item->m_setting);

	QUndoCommand* cs = nullptr;
	if (ui->valueRadioButton->isChecked()) {
		auto w = dynamic_cast<ColorMapSettingEditWidgetI*> (ui->colorMapWidget->widget());
		auto cs_com = w->createModifyCommand();
		cs = m_item->gridTypeDataItem()->createApplyColorMapSettingAndRenderCommand(colorTarget(), cs_com, apply);
	}

	return new UpdateDisplaySettingCommand(apply, s, cs);
}

void GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::accept()
{
	m_item->pushUpdateActorSettingsCommand(createModifyCommand(false));
	QDialog::accept();
}

void GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::colorTargetChanged(int index)
{
	auto oldWidget = ui->colorMapWidget->widget();
	if (oldWidget != nullptr) {
		delete oldWidget;
	}

	auto colorTarget = m_colorTargets.at(index);
	auto att = m_gridType->gridAttribute(colorTarget);
	auto w = att->createColorMapSettingEditWidget(this);

	auto legend = m_item->m_colorMapLegendSettings.at(colorTarget);
	w->setLegendSetting(legend);

	ui->colorMapWidget->setWidget(w);

	connect(ui->importButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, w, &ColorMapSettingEditWidgetI::exportSetting);
}

void GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

GridBirdEyeWindowSetting GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::setting() const
{
	GridBirdEyeWindowSetting ret;

	if (m_warpTargets.size() > 0) {
		ret.warpTarget = m_warpTargets.at(ui->elevationComboBox->currentIndex()).c_str();
	}
	if (m_colorTargets.size() > 0) {
		ret.colorTarget = m_colorTargets.at(ui->valueComboBox->currentIndex()).c_str();
	}

	if (ui->valueRadioButton->isChecked()) {
		ret.mapping = GridBirdEyeWindowSetting::Mapping::Value;
	} else {
		ret.mapping = GridBirdEyeWindowSetting::Mapping::Arbitrary;
	}
	ret.customColor = ui->customColorWidget->color();
	ret.showAxes = ui->axesVisibleCheckBox->isChecked();
	ret.axesColor = ui->axesColorWidget->color();
	ret.zScale = ui->zScaleSpinBox->value();
	ret.backgroundColor = ui->bgColorWidget->color();

	return ret;
}

void GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::setSetting(const GridBirdEyeWindowSetting& setting)
{
	if (setting.mapping == GridBirdEyeWindowSetting::Mapping::Value) {
		ui->valueRadioButton->setChecked(true);
	} else if (setting.mapping == GridBirdEyeWindowSetting::Mapping::Arbitrary) {
		ui->customColorRadioButton->setChecked(true);
	}
	ui->customColorWidget->setColor(setting.customColor);
	ui->axesVisibleCheckBox->setChecked(setting.showAxes);
	ui->axesColorWidget->setColor(setting.axesColor);
	ui->zScaleSpinBox->setValue(setting.zScale);
	ui->bgColorWidget->setColor(setting.backgroundColor);

	auto target = iRIC::toStr(setting.warpTarget);
	for (int i = 0; i < m_warpTargets.size(); ++i) {
		if (target != m_warpTargets[i]) {continue;}

		ui->elevationComboBox->setCurrentIndex(i);
		break;
	}

	ui->valueComboBox->blockSignals(true);
	target = iRIC::toStr(setting.colorTarget);
	for (int i = 0; i < m_colorTargets.size(); ++i) {
		if (target != m_colorTargets[i]) {continue;}
		ui->valueComboBox->setCurrentIndex(i);
		colorTargetChanged(i);
		break;
	}
	ui->valueComboBox->blockSignals(false);
}

void GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::apply()
{
	m_item->pushUpdateActorSettingsCommand(createModifyCommand(true));
	m_applied = true;
}

void GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::setupWarpTargets()
{
	for (const auto& att : m_gridType->gridAttributes()) {
		if (att->position() == SolverDefinitionGridAttribute::CellCenter) {continue;}
		if (att->isOption()) {continue;}

		ui->elevationComboBox->addItem(att->caption());
		m_warpTargets.push_back(att->name());
	}
}

void GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::setupColorTargets()
{
	ui->valueComboBox->blockSignals(true);

	for (const auto& att : m_gridType->gridAttributes()) {
		ui->valueComboBox->addItem(att->caption());
		m_colorTargets.push_back(att->name());
	}

	ui->valueComboBox->blockSignals(false);
}

std::string GridBirdEyeWindowGridDataItem::EditDisplaySettingDialog::colorTarget() const
{
	return m_colorTargets.at(ui->valueComboBox->currentIndex());
}
