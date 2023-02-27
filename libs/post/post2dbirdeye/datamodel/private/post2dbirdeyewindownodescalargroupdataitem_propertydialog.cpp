#include "../post2dbirdeyewindowgridtypedataitem.h"
#include "../post2dbirdeyewindownodescalargrouptopdataitem.h"
#include "../post2dbirdeyewindowzonedataitem.h"
#include "post2dbirdeyewindownodescalargroupdataitem_propertydialog.h"
#include "post2dbirdeyewindownodescalargroupdataitem_updateactorsettingscommand.h"
#include "ui_post2dbirdeyewindownodescalargroupdataitem_propertydialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitionoutput.h>
#include <misc/iricundostack.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

#include <map>

Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::PropertyDialog(Post2dBirdEyeWindowNodeScalarGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_item {item},
	ui(new Ui::Post2dBirdEyeWindowNodeScalarGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);

	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect<void(QComboBox::*)(int)>(ui->scalarComboBox, &QComboBox::currentIndexChanged, this, &PropertyDialog::handleColorScalarChange);
	connect(ui->importButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->exportSetting();});

	auto gType = m_item->topDataItem()->zoneDataItem()->gridTypeDataItem()->gridType();

	std::map<QString, std::string> captionMap;
	for (const auto& pair : item->m_colorMapSettings) {
		auto name = pair.first;
		auto caption = gType->output(name)->caption();
		captionMap.insert({caption, name});
	}

	ui->scalarComboBox->blockSignals(true);
	for (const auto& pair : captionMap) {
		ui->scalarComboBox->addItem(pair.first);
		m_colorMapNames.push_back(pair.second);
	}
	ui->scalarComboBox->blockSignals(false);

	auto container = item->topDataItem()->zoneDataItem()->dataContainer();
	if (! container->IBCExists()) {
		ui->rangeWidget->disableActive();
	}

	auto data = container->data()->data();
	auto sgrid = vtkStructuredGrid::SafeDownCast(data);
	if (sgrid == nullptr) {
		ui->rangeWidget->hideCustom();
	} else {
		int dims[3];
		sgrid->GetDimensions(dims);
		ui->rangeWidget->setDimensions(dims[0], dims[1]);
	}

	setSetting(item->m_setting);
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::createModifyCommand(bool apply)
{
	auto settingCommand = new ValueModifyCommmand<Setting>(iRIC::generateCommandId("ModifySetting"), true, setting(), &m_item->m_setting);
	QUndoCommand* colorMapCommand = nullptr;

	if (m_colorMapNames.size() > 0) {
		colorMapCommand = ui->colorMapWidget->createModifyCommand();
	}

	return new UpdateActorSettingsCommand(apply, settingCommand, colorMapCommand, m_item);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::accept()
{
	m_item->pushRenderCommand(createModifyCommand(false), m_item, true);

	QDialog::accept();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::reject();
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::handleColorScalarChange(int index)
{
	auto name = m_colorMapNames.at(index);
	auto cs = m_item->m_colorMapSettings.find(name)->second;

	ui->colorMapWidget->setSetting(cs);
}

Post2dBirdEyeWindowNodeScalarGroupDataItem::Setting Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::setting() const
{
	Setting setting;

	setting.regionSetting = ui->rangeWidget->setting();

	if (ui->specificRadioButton->isChecked()) {
		setting.colorMode = Setting::ColorMode::Custom;
	} else {
		setting.colorMode = Setting::ColorMode::ByScalar;
	}
	setting.customColor = ui->colorEditWidget->color();
	if (m_colorMapNames.size() > 0) {
		setting.colorTarget = m_colorMapNames.at(ui->scalarComboBox->currentIndex()).c_str();
	}
	setting.opacity = ui->opacityWidget->opacityPercent();

	return setting;
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::setSetting(const Setting& setting)
{
	ui->rangeWidget->setSetting(setting.regionSetting);
	if (setting.colorMode == Setting::ColorMode::Custom) {
		ui->specificRadioButton->setChecked(true);
	} else {
		ui->scalarRadioButton->setChecked(true);
	}
	ui->colorEditWidget->setColor(setting.customColor);
	for (int i = 0; i < m_colorMapNames.size(); ++i) {
		if (setting.colorTarget == m_colorMapNames.at(i).c_str()) {
			ui->scalarComboBox->blockSignals(true);
			ui->scalarComboBox->setCurrentIndex(i);
			handleColorScalarChange(i);
			ui->scalarComboBox->blockSignals(false);
		}
	}
	ui->opacityWidget->setOpacityPercent(setting.opacity);
}

void Post2dBirdEyeWindowNodeScalarGroupDataItem::PropertyDialog::apply()
{
	m_item->pushRenderCommand(createModifyCommand(true), m_item, true);
}
