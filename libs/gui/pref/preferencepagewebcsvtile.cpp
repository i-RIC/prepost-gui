#include "preferencepagewebcsvtile.h"
#include "ui_preferencepagewebcsvtile.h"
#include "private/preferencepagewebcsvtileadddialog.h"

#include <geodata/pointmap/geodatapointmapwebimportersettingmanager.h>

PreferencePageWebCsvTile::PreferencePageWebCsvTile(QWidget *parent) :
	PreferencePage {parent},
	ui {new Ui::PreferencePageWebCsvTile}
{
	ui->setupUi(this);

	GeoDataPointmapWebImporterSettingManager manager;
	m_settings = manager.settings();

	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(edit()));
	connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));
	connect(ui->defaultButton, SIGNAL(clicked()), this, SLOT(restoreDefault()));

	connect(ui->upButton, SIGNAL(clicked()), this, SLOT(moveUpSelected()));
	connect(ui->downButton, SIGNAL(clicked()), this, SLOT(moveDownSelected()));

	connect(ui->listWidget, SIGNAL(currentRowChanged(int)), this, SLOT(handleListWidgetSelectChange(int)));
	updateList();
}

PreferencePageWebCsvTile::~PreferencePageWebCsvTile()
{
	delete ui;
}

void PreferencePageWebCsvTile::update()
{
	GeoDataPointmapWebImporterSettingManager manager;
	manager.setSettings(m_settings);
}

void PreferencePageWebCsvTile::add()
{
	PreferencePageWebCsvTileAddDialog dialog(this);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_settings.push_back(dialog.setting());
	updateList();

	ui->listWidget->setCurrentRow(ui->listWidget->count() - 1);
}

void PreferencePageWebCsvTile::edit()
{
	int idx = ui->listWidget->currentRow();

	GeoDataPointmapWebImporterSetting oldSetting = m_settings[idx];

	PreferencePageWebCsvTileAddDialog dialog(this);
	dialog.setWindowTitle(tr("Edit Web Elevation Data"));
	dialog.setName(oldSetting.caption());
	dialog.setMinZoom(oldSetting.minZoomLevel());
	dialog.setMaxZoom(oldSetting.maxZoomLevel());
	dialog.setUrl(oldSetting.url());

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto newSetting = dialog.setting();
	m_settings[idx] = newSetting;

	updateList();
	ui->listWidget->setCurrentRow(idx);
}

void PreferencePageWebCsvTile::deleteSelected()
{
	int row = ui->listWidget->currentRow();
	if (row == -1) {return;}

	m_settings.erase(m_settings.begin() + row);
	updateList();

	if (row >= ui->listWidget->count()){
		row = ui->listWidget->count() - 1;
	}
	ui->listWidget->setCurrentRow(row);
}

void PreferencePageWebCsvTile::restoreDefault()
{
	GeoDataPointmapWebImporterSettingManager  manager;
	m_settings = manager.defaultSettings();

	updateList();
}

void PreferencePageWebCsvTile::moveUpSelected()
{
	int row = ui->listWidget->currentRow();
	if (row < 1) {return;}

	auto s = m_settings.at(row);
	m_settings.erase(m_settings.begin() + row);
	m_settings.insert(m_settings.begin() + row - 1, s);

	updateList();
	ui->listWidget->setCurrentRow(row - 1);
}

void PreferencePageWebCsvTile::moveDownSelected()
{
	int row = ui->listWidget->currentRow();
	if (row == ui->listWidget->count() - 1) {return;}

	auto s = m_settings.at(row);
	m_settings.erase(m_settings.begin() + row);
	m_settings.insert(m_settings.begin() + row + 1, s);

	updateList();
	ui->listWidget->setCurrentRow(row + 1);
}

void PreferencePageWebCsvTile::handleListWidgetSelectChange(int current)
{
	if (current < 0 || current >= m_settings.size()) {
		ui->editButton->setEnabled(false);
		ui->deleteButton->setEnabled(false);
		return;
	}
	ui->editButton->setEnabled(true);
	ui->deleteButton->setEnabled(true);
}

void PreferencePageWebCsvTile::updateList()
{
	ui->listWidget->clear();
	for (const auto& setting : m_settings) {
		auto item = new QListWidgetItem(setting.caption());
		ui->listWidget->addItem(item);
	}
}
