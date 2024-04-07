#include "geodatariversurveywebimporter_settingdialog.h"
#include "ui_geodatariversurveywebimporter_settingdialog.h"

#include <misc/qttool.h>
#include <misc/stringtool.h>

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QPushButton>
#include <QRegularExpression>

#include <algorithm>
#include <map>
#include <set>

GeoDataRiverSurveyWebImporter::SettingDialog::SettingDialog(QWidget *parent) :
	QDialog(parent),
	m_webAccessManager {QtTool::networkAccessManager()},
	m_webReply {nullptr},
	ui(new Ui::GeoDataRiverSurveyWebImporter_SettingDialog)
{
	ui->setupUi(this);

	connect<void (QComboBox::*)(int)>(ui->riverComboBox, &QComboBox::currentIndexChanged, this, &SettingDialog::readKPs);
	connect<void (QComboBox::*)(int)>(ui->minComboBox, &QComboBox::currentIndexChanged, this, &SettingDialog::readYears);
	connect<void (QComboBox::*)(int)>(ui->maxComboBox, &QComboBox::currentIndexChanged, this, &SettingDialog::readYears);

	readRiverIds();
}

GeoDataRiverSurveyWebImporter::SettingDialog::~SettingDialog()
{
	delete ui;
}

GeoDataRiverSurveyWebImporter::Setting GeoDataRiverSurveyWebImporter::SettingDialog::setting() const
{
	Setting ret;

	ret.riverId = m_riverIds.at(ui->riverComboBox->currentIndex());
	ret.maxKp = m_KPs.at(ui->maxComboBox->currentIndex());
	ret.minKp = m_KPs.at(ui->minComboBox->currentIndex());
	ret.year = m_years.at(ui->yearComboBox->currentIndex());

	return ret;
}

void GeoDataRiverSurveyWebImporter::SettingDialog::readRiverIds()
{
	QUrl url("https://rivopen.ricplace2.org/api/riverlist");
	QNetworkRequest request(url);
	m_webReply = m_webAccessManager->get(request);
	m_isWaitingHttpResponse = true;
	disableWidgetsAndShowLoadingMessage();
	connect(m_webReply, &QNetworkReply::finished, this, &SettingDialog::handleDone);
	qApp->processEvents();

	while (m_isWaitingHttpResponse) {
		qApp->thread()->wait(100);
		qApp->processEvents();
	}

	if (m_webReply->error() != QNetworkReply::NoError) {
		const auto message = QString(tr("Server returned error response: %1").arg(m_webReply->errorString()));
		enableWidgetsOtherthanOkButtonAndShowMessage(message);
		return;
	}

	auto buffer = m_webReply->readAll();
	auto string = buffer.toStdString();

	auto data = QJsonDocument::fromJson(buffer);
	auto list = data["riverlist"].toObject();

	ui->riverComboBox->clear();
	m_riverIds.clear();
	std::map<std::string, QString> riverIdMap;

	for (auto it = list.begin(); it != list.end(); ++it) {
		auto name = it.key();
		auto id = iRIC::toStr(it.value().toString());
		riverIdMap.insert({id, name});
	}

	ui->riverComboBox->blockSignals(true);

	for (const auto& pair : riverIdMap) {
		m_riverIds.push_back(pair.first.c_str());
		ui->riverComboBox->addItem(pair.second);
	}
	ui->riverComboBox->setCurrentIndex(0);

	ui->riverComboBox->blockSignals(false);

	enableWidgets();

	readKPs();
}

void GeoDataRiverSurveyWebImporter::SettingDialog::readKPs()
{
	auto riverId = m_riverIds.at(ui->riverComboBox->currentIndex());

	QUrl url("https://rivopen.ricplace2.org/api/riverkp/" + riverId);

	QNetworkRequest request(url);
	m_webReply = m_webAccessManager->get(request);
	m_isWaitingHttpResponse = true;
	disableWidgetsAndShowLoadingMessage();
	connect(m_webReply, &QNetworkReply::finished, this, &SettingDialog::handleDone);
	qApp->processEvents();

	while (m_isWaitingHttpResponse) {
		qApp->thread()->wait(100);
		qApp->processEvents();
	}

	if (m_webReply->error() != QNetworkReply::NoError) {
		const auto message = QString(tr("Server returned error response: %1").arg(m_webReply->errorString()));
		enableWidgetsOtherthanOkButtonAndShowMessage(message);
		return;
	}

	auto buffer = QString(m_webReply->readAll());
	QRegularExpression ex("([0-9\\.\\-]+)(,|\\r|\\n)");
	auto buffer2 = buffer.replace(ex, "\"\\1\"\\2");

	auto data = QJsonDocument::fromJson(buffer2.toUtf8());
	auto list = data["kplist"].toArray();

	int minNonNegativeIndex = -1;
	m_KPs.clear();
	for (auto it = list.begin(); it != list.end(); ++it) {
		auto kp = *it;
		m_KPs.push_back(kp.toString());
		if (minNonNegativeIndex < 0 && kp.toString().toFloat() >= 0) {
			minNonNegativeIndex = m_KPs.size() - 1;
		}
	}

	ui->maxComboBox->blockSignals(true);
	ui->minComboBox->blockSignals(true);

	ui->maxComboBox->clear();
	ui->minComboBox->clear();
	for (const auto& kp : m_KPs) {
		ui->minComboBox->addItem(kp);
		ui->maxComboBox->addItem(kp);
	}

	ui->maxComboBox->setCurrentIndex(minNonNegativeIndex);
	ui->minComboBox->setCurrentIndex(minNonNegativeIndex);

	ui->minComboBox->blockSignals(false);
	ui->maxComboBox->blockSignals(false);

	enableWidgets();

	readYears();
}

void GeoDataRiverSurveyWebImporter::SettingDialog::readYears()
{
	auto riverId = m_riverIds.at(ui->riverComboBox->currentIndex());
	auto minKP = m_KPs.at(ui->minComboBox->currentIndex());
	auto maxKP = m_KPs.at(ui->maxComboBox->currentIndex());

	QUrl url("https://rivopen.ricplace2.org/api/riveryear/" + riverId + "/" + minKP + "/" + maxKP);

	m_years.clear();
	ui->yearComboBox->clear();

	QNetworkRequest request(url);
	m_webReply = m_webAccessManager->get(request);
	m_isWaitingHttpResponse = true;
	disableWidgetsAndShowLoadingMessage();
	connect(m_webReply, &QNetworkReply::finished, this, &SettingDialog::handleDone);
	qApp->processEvents();

	while (m_isWaitingHttpResponse) {
		qApp->thread()->wait(100);
		qApp->processEvents();
	}

	if (m_webReply->error() != QNetworkReply::NoError) {
		const auto message = QString(tr("Server returned error response: %1").arg(m_webReply->errorString()));
		enableWidgetsOtherthanOkButtonAndShowMessage(message);
		return;
	}

	auto buffer = QString(m_webReply->readAll());
	QRegularExpression ex("([0-9\\.\\-]+)(,| |\\r|\\n)");
	auto buffer2 = buffer.replace(ex, "\"\\1\"\\2");

	auto data = QJsonDocument::fromJson(buffer2.toUtf8());
	auto list = data.object();

	std::set<QString> yearSet;
	for (auto it = list.begin(); it != list.end(); ++it) {
		auto kp = it.key();
		auto yearList = it.value().toArray();
		for (auto it2 = yearList.begin(); it2 != yearList.end(); ++it2) {
			yearSet.insert(it2->toString());
		}
	}

	for (const auto& year : yearSet) {
		m_years.push_back(year);
		ui->yearComboBox->addItem(year);
	}
	ui->yearComboBox->setCurrentIndex(m_years.size() - 1);

	enableWidgets();
}

void GeoDataRiverSurveyWebImporter::SettingDialog::handleDone()
{
	m_isWaitingHttpResponse = false;
}

void GeoDataRiverSurveyWebImporter::SettingDialog::enableWidgets()
{
	ui->riverComboBox->setEnabled(true);
	ui->maxComboBox->setEnabled(true);
	ui->minComboBox->setEnabled(true);
	ui->yearComboBox->setEnabled(true);
	auto okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setEnabled(true);

	ui->messageLabel->setText("");
}

void GeoDataRiverSurveyWebImporter::SettingDialog::disableWidgetsAndShowLoadingMessage()
{
	ui->riverComboBox->setDisabled(true);
	ui->maxComboBox->setDisabled(true);
	ui->minComboBox->setDisabled(true);
	ui->yearComboBox->setDisabled(true);
	auto okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setDisabled(true);

	ui->messageLabel->setText(tr("Waiting for response from server..."));
}

void GeoDataRiverSurveyWebImporter::SettingDialog::enableWidgetsOtherthanOkButtonAndShowMessage(const QString& message)
{
	ui->riverComboBox->setEnabled(true);
	ui->maxComboBox->setEnabled(true);
	ui->minComboBox->setEnabled(true);
	ui->yearComboBox->setEnabled(true);
	auto okButton = ui->buttonBox->button(QDialogButtonBox::Ok);
	okButton->setDisabled(true);

	ui->messageLabel->setText(message);
}
