#include "preferencepagewindowsize.h"
#include "ui_preferencepagewindowsize.h"

#include <misc/windowsizestandardsettingeditdialog.h>

#include <QMessageBox>
#include <QSettings>
#include <QStandardItem>

PreferencePageWindowSize::PreferencePageWindowSize(QWidget *parent) :
	PreferencePage {parent},
	m_model {},
	m_selectionModel {&m_model},
	ui(new Ui::PreferencePageWindowSize)
{
	ui->setupUi(this);
	ui->listView->setModel(&m_model);
	ui->listView->setSelectionModel(&m_selectionModel);

	QSettings settings;
	auto s = settings.value("standardWindowSizes");
	auto list = s.toStringList();

	if (! list.isEmpty()) {
		for (const auto& str : list) {
			WindowSizeStandardSetting setting;
			setting.fromString(str);
			m_standardSettings.push_back(setting);
		}
	} else {
		m_standardSettings = WindowSizeStandardSetting::buildDefaultSettings();
	}

	updateList();

	connect(ui->addButton, &QPushButton::clicked, this, &PreferencePageWindowSize::add);
	connect(ui->editButton, &QPushButton::clicked, this, &PreferencePageWindowSize::editSelected);
	connect(ui->deleteButton, &QPushButton::clicked, this, &PreferencePageWindowSize::deleteSelected);
	connect(ui->resetButton, &QPushButton::clicked, this, &PreferencePageWindowSize::reset);
}

PreferencePageWindowSize::~PreferencePageWindowSize()
{
	delete ui;
}

void PreferencePageWindowSize::update()
{
	QStringList data;
	for (const auto& s : m_standardSettings) {
		data.push_back(s.toString());
	}

	QSettings settings;
	settings.setValue("standardWindowSizes", data);
}

void PreferencePageWindowSize::add()
{
	WindowSizeStandardSetting s;
	s.width = 640;
	s.height = 480;
	s.comment = "";

	WindowSizeStandardSettingEditDialog dialog(this);
	dialog.setSetting(s);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto newSetting = dialog.setting();
	m_standardSettings.push_back(newSetting);
	std::sort(m_standardSettings.begin(), m_standardSettings.end());

	updateList();

	auto it = std::find(m_standardSettings.begin(), m_standardSettings.end(), newSetting);
	auto row = it - m_standardSettings.begin();
	m_selectionModel.select(m_model.index(row, 0), QItemSelectionModel::Select);
}

void PreferencePageWindowSize::editSelected()
{
	if (m_selectionModel.selectedIndexes().size() == 0) {return;}

	auto row = m_selectionModel.selectedIndexes().at(0).row();
	const auto& s = m_standardSettings.at(row);

	WindowSizeStandardSettingEditDialog dialog(this);
	dialog.setSetting(s);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	auto newSetting = dialog.setting();
	m_standardSettings[row] = newSetting;
	std::sort(m_standardSettings.begin(), m_standardSettings.end());

	updateList();

	auto it = std::find(m_standardSettings.begin(), m_standardSettings.end(), newSetting);
	row = it - m_standardSettings.begin();
	m_selectionModel.select(m_model.index(row, 0), QItemSelectionModel::Select);
}

void PreferencePageWindowSize::deleteSelected()
{
	if (m_selectionModel.selectedIndexes().size() == 0) {return;}

	auto row = m_selectionModel.selectedIndexes().at(0).row();
	const auto& s = m_standardSettings.at(row);

	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete \"%1\"?").arg(s.labelString()), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	auto it = m_standardSettings.begin() + row;
	m_standardSettings.erase(it);

	updateList();

	if (row >= static_cast<int> (m_standardSettings.size())) {
		row = static_cast<int> (m_standardSettings.size()) - 1;
	}
	m_selectionModel.select(m_model.index(row, 0), QItemSelectionModel::Select);
}

void PreferencePageWindowSize::reset()
{
	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to reset to default settings?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	m_standardSettings = WindowSizeStandardSetting::buildDefaultSettings();

	updateList();
}

void PreferencePageWindowSize::updateList()
{
	m_model.setColumnCount(1);
	m_model.setRowCount(static_cast<int> (m_standardSettings.size()));
	for (int i = 0; i < static_cast<int> (m_standardSettings.size()); ++i) {
		const auto& s = m_standardSettings.at(i);
		auto item = new QStandardItem(s.labelString());
		m_model.setItem(i, 0, item);
	}
}
