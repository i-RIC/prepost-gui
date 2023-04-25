#include "../post3dwindowgridtypedataitem.h"
#include "../post3dwindowparticlesbasescalardataitem.h"
#include "../post3dwindowparticlesbasescalargroupdataitem.h"
#include "post3dwindowparticlesbasescalargroupdataitem_propertydialog.h"
#include "post3dwindowparticlesbasescalargroupdataitem_updateactorsettingcommand.h"
#include "ui_post3dwindowparticlesbasescalargroupdataitem_propertydialog.h"

#include <guibase/vtkdatasetattributestool.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::PropertyDialog(Post3dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_item {item},
	ui(new Ui::Post3dWindowParticlesBaseScalarGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect<void(QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &PropertyDialog::colorTargetChanged);
	connect(ui->importButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->exportSetting();});

	setupColorTargets();

	setSetting(item->m_setting);
}

Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto settingCommand = new ValueModifyCommmand<ParticleDataSetting>("Edit setting", setting(), &m_item->m_setting);
	QUndoCommand* colorMapCommand = nullptr;
	if (ui->valueRadioButton->isChecked()) {
		colorMapCommand = ui->colorMapWidget->createModifyCommand();
	}
	return new UpdateActorSettingCommand(apply, settingCommand, colorMapCommand, m_item);
}

void Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);
	QDialog::accept();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}
	QDialog::reject();
}

void Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::colorTargetChanged(int index)
{
	auto colorTarget = m_colorTargets.at(index);
	auto child = m_item->childDataItem(colorTarget);
	auto& cs = child->colorMapSetting();

	ui->colorMapWidget->setSetting(&cs);
}

void Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

ParticleDataSetting Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::setting() const
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

void Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::setSetting(const ParticleDataSetting& setting)
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

void Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
	m_applied = true;
}

void Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::setupColorTargets()
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
	}
}

std::string Post3dWindowParticlesBaseScalarGroupDataItem::PropertyDialog::colorTarget() const
{
	return m_colorTargets.at(ui->valueComboBox->currentIndex());
}
