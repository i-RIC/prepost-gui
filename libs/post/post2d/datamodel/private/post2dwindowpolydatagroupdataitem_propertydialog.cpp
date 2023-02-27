#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowzonedataitem.h"
#include "post2dwindowpolydatagroupdataitem_propertydialog.h"
#include "post2dwindowpolydatagroupdataitem_updateactorsettingscommand.h"
#include "ui_post2dwindowpolydatagroupdataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

#include <vtkCellData.h>
#include <vtkPolyData.h>

Post2dWindowPolyDataGroupDataItem::PropertyDialog::PropertyDialog(Post2dWindowPolyDataGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post2dWindowPolyDataGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect<void(QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &PropertyDialog::colorTargetChanged);
	connect(ui->importButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->exportSetting();});

	setupColorTargets();

	setSetting(item->m_setting);
}

Post2dWindowPolyDataGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dWindowPolyDataGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto settingCommand = new ValueModifyCommmand<PolyDataSetting>("Edit setting", setting(), &m_item->m_setting);
	QUndoCommand* colorMapCommand = nullptr;
	if (ui->valueRadioButton->isChecked()) {
		colorMapCommand = ui->colorMapWidget->createModifyCommand();
	}
	return new UpdateActorSettingsCommand(apply, settingCommand, colorMapCommand, m_item);
}

void Post2dWindowPolyDataGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(), m_item, true);
	QDialog::accept();
}

void Post2dWindowPolyDataGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void Post2dWindowPolyDataGroupDataItem::PropertyDialog::colorTargetChanged(int index)
{
	auto colorTarget = m_colorTargets.at(index);
	auto cs = m_item->m_colorMapSettings.at(colorTarget);
	ui->colorMapWidget->setSetting(cs);
}

void Post2dWindowPolyDataGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

PolyDataSetting Post2dWindowPolyDataGroupDataItem::PropertyDialog::setting() const
{
	PolyDataSetting ret;

	if (ui->customColorRadioButton->isChecked()) {
		ret.mapping = PolyDataSetting::Mapping::Arbitrary;
	} else {
		ret.mapping = PolyDataSetting::Mapping::Value;
	}
	ret.color = ui->customColorWidget->color();
	if (m_colorTargets.size() > 0) {
		ret.value = colorTarget().c_str();
	}
	ret.opacity = ui->transparencyWidget->opacityPercent();
	ret.lineWidth = ui->lineWidthSpinBox->value();

	return ret;
}

void Post2dWindowPolyDataGroupDataItem::PropertyDialog::setSetting(const PolyDataSetting& setting)
{
	if (setting.mapping == PolyDataSetting::Mapping::Arbitrary) {
		ui->customColorRadioButton->setChecked(true);
	} else {
		ui->valueRadioButton->setChecked(true);
	}
	ui->customColorWidget->setColor(setting.color);
	auto value = iRIC::toStr(setting.value);
	for (int i = 0; i < m_colorTargets.size(); ++i) {
		if (value == m_colorTargets.at(i)) {
			ui->valueComboBox->setCurrentIndex(i);
			colorTargetChanged(i);
		}
	}
	ui->transparencyWidget->setOpacityPercent(setting.opacity);
	ui->lineWidthSpinBox->setValue(setting.lineWidth);
}

void Post2dWindowPolyDataGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
	m_applied = true;
}

void Post2dWindowPolyDataGroupDataItem::PropertyDialog::setupColorTargets()
{
	ui->valueComboBox->blockSignals(true);

	auto data = m_item->polyData();
	for (auto const& name : vtkDataSetAttributesTool::getArrayNames(data->GetCellData())) {
		ui->valueComboBox->addItem(name.c_str());
		m_colorTargets.push_back(name);
	}

	ui->valueComboBox->blockSignals(false);

	if (m_colorTargets.size() == 0) {
		ui->customColorRadioButton->setChecked(true);
		ui->valueRadioButton->setDisabled(true);
	}
}

std::string Post2dWindowPolyDataGroupDataItem::PropertyDialog::colorTarget() const
{
	return m_colorTargets.at(ui->valueComboBox->currentIndex());
}
