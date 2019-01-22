#include "preferencepageriversurvey.h"
#include "ui_preferencepageriversurvey.h"
#include "private/preferencepageriversurveyfractionadddialog.h"

#include <QMessageBox>
#include <QString>

#include <algorithm>

PreferencePageRiverSurvey::PreferencePageRiverSurvey(QWidget *parent) :
	PreferencePage(parent),
	ui (new Ui::PreferencePageRiverSurvey)
{
	ui->setupUi(this);
	connect(ui->addButton, SIGNAL(clicked()), this, SLOT(add()));
	connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteSelected()));
	connect(ui->defaultButton, SIGNAL(clicked()), this, SLOT(restoreDefault()));

	auto fractions = m_settings.value("riversurvey/slopeangles", QString("0.5,1,2,3,4,5,10,20")).toString().split(",");

	m_fractions.clear();
	for (auto f : fractions) {
		m_fractions.push_back(f.toDouble());
	}
	std::sort(m_fractions.begin(), m_fractions.end());

	updateList();
}

PreferencePageRiverSurvey::~PreferencePageRiverSurvey()
{
	delete ui;
}

void PreferencePageRiverSurvey::update()
{
	QStringList strFractions;
	for (auto f : m_fractions) {
		strFractions.push_back(QString::number(f));
	}
	m_settings.setValue("riversurvey/slopeangles", strFractions.join(","));
}

void PreferencePageRiverSurvey::add()
{
	PreferencePageRiverSurveyFractionAddDialog dialog(this);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	double f = dialog.fraction();

	int idx = 0;
	while (idx < m_fractions.size() && f > m_fractions.at(idx)) {
		++idx;
	}

	if (idx < m_fractions.size() && f == m_fractions.at(idx)) {
		QMessageBox::information(this, tr("Information"), tr("This value already exists in the list."));
		return;
	}
	m_fractions.insert(m_fractions.begin() + idx, f);

	updateList();
	ui->listWidget->setCurrentRow(idx);
}

void PreferencePageRiverSurvey::deleteSelected()
{
	auto selectedRows = ui->listWidget->selectionModel()->selectedRows();
	if (selectedRows.size() == 0) {return;}

	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to delete the selected item?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	int selectedRow = selectedRows.at(0).row();
	auto it = m_fractions.begin() + selectedRow;
	m_fractions.erase(it);

	updateList();
}

void PreferencePageRiverSurvey::restoreDefault()
{
	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to restore default values?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	m_fractions.clear();
	m_fractions.push_back(0.5);
	m_fractions.push_back(1);
	m_fractions.push_back(2);
	m_fractions.push_back(3);
	m_fractions.push_back(4);
	m_fractions.push_back(5);
	m_fractions.push_back(10);
	m_fractions.push_back(20);

	updateList();
}

void PreferencePageRiverSurvey::updateList()
{
	ui->listWidget->clear();

	for (double f : m_fractions) {
		ui->listWidget->addItem(QString::number(f));
	}
}
