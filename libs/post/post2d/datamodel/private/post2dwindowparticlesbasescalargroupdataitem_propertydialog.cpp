#include "../post2dwindowgridtypedataitem.h"
#include "../post2dwindowparticlesbasescalardataitem.h"
#include "../post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem_propertydialog.h"
#include "post2dwindowparticlesbasescalargroupdataitem_updateactorsettingcommand.h"
#include "ui_post2dwindowparticlesbasescalargroupdataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::PropertyDialog(Post2dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post2dWindowParticlesBaseScalarGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect<void(QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &PropertyDialog::colorTargetChanged);

	m_colorMapWidget = new ColorMapSettingEditWidget(this);
	auto widget = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapWidget, this);
	ui->colorMapWidget->setWidget(widget);

	setupColorTargets();

	setSetting(item->m_setting);
}

Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto settingCommand = new ValueModifyCommmand<ParticleDataSetting>("Edit setting", setting(), &m_item->m_setting);
	QUndoCommand* colorMapCommand = nullptr;
	if (ui->valueRadioButton->isChecked()) {
		colorMapCommand = m_colorMapWidget->createModifyCommand();
	}
	return new UpdateActorSettingCommand(apply, settingCommand, colorMapCommand, m_item);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(), m_item, true);
	QDialog::accept();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::colorTargetChanged(int index)
{
	auto colorTarget = m_colorTargets.at(index);
	auto child = m_item->childDataItem(colorTarget);
	auto& cs = child->colorMapSetting();

	m_colorMapWidget->setSetting(&cs);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

ParticleDataSetting Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::setting() const
{
	ParticleDataSetting ret;

	if (ui->customColorRadioButton->isChecked()) {
		ret.mapping = ParticleDataSetting::Mapping::Arbitrary;
	} else {
		ret.mapping = ParticleDataSetting::Mapping::Value;
	}
	ret.color = ui->customColorWidget->color();
	if (m_colorTargets.size() > 0) {
		ret.value = colorTarget().c_str();
	}
	ret.opacity = ui->transparencyWidget->opacityPercent();
	ret.particleSize = ui->pointSizeSpinBox->value();

	return ret;
}

void Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::setSetting(const ParticleDataSetting& setting)
{
	if (setting.mapping == ParticleDataSetting::Mapping::Arbitrary) {
		ui->customColorRadioButton->setChecked(true);
	} else {
		ui->valueRadioButton->setChecked(true);
	}
	ui->customColorWidget->setColor(setting.color);
	auto value = iRIC::toStr(setting.value);
	if (value == "" && m_colorTargets.size() > 0) {
		value = m_colorTargets.at(0);
	}
	for (int i = 0; i < m_colorTargets.size(); ++i) {
		if (value == m_colorTargets.at(i)) {
			ui->valueComboBox->setCurrentIndex(i);
			colorTargetChanged(i);
		}
	}
	ui->transparencyWidget->setOpacityPercent(setting.opacity);
	ui->pointSizeSpinBox->setValue(setting.particleSize);
}

void Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
	m_applied = true;
}

void Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::setupColorTargets()
{
	ui->valueComboBox->blockSignals(true);

	auto data = m_item->particleData();
	for (auto const& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(data->GetPointData())) {
		ui->valueComboBox->addItem(name.c_str());
		m_colorTargets.push_back(name);
	}

	ui->valueComboBox->blockSignals(false);

	if (m_colorTargets.size() == 0) {
		ui->customColorRadioButton->setChecked(true);
		ui->customColorRadioButton->setDisabled(true);
		ui->valueRadioButton->setDisabled(true);
		ui->colorMapGroupBox->setDisabled(true);
	}
}

std::string Post2dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::colorTarget() const
{
	return m_colorTargets.at(ui->valueComboBox->currentIndex());
}
