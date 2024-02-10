#include "geodatariversurveymlitimporter.h"
#include "geodatariversurveywebimporter.h"
#include "private/geodatariversurveywebimporter_settingdialog.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodatacreator.h>
#include <guicore/project/projectdata.h>
#include <misc/fileremover.h>
#include <misc/filesystemfunction.h>
#include <misc/ziparchive.h>
#include <misc/qttool.h>

#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>

GeoDataRiverSurveyWebImporter::GeoDataRiverSurveyWebImporter(GeoDataCreator* creator) :
	GeoDataWebImporter("ricsite", tr("Cross-Section Data from Profile of Hokkaido Development"), creator),
	m_webAccessManager {QtTool::networkAccessManager()},
	m_webReply {nullptr},
	m_isWaitingHttpResponse {false},
	m_mlitImporter {nullptr}
{
	for (auto importer : creator->importers()) {
		auto mlitImporter = dynamic_cast<GeoDataRiverSurveyMlitImporter*> (importer);
		if (mlitImporter != nullptr) {
			m_mlitImporter = mlitImporter;
		}
	}
}

GeoDataRiverSurveyWebImporter::~GeoDataRiverSurveyWebImporter()
{
	QFile tmp(m_tmpFile);
	if (tmp.exists()) {
		tmp.remove();
	}

	iRIC::rmdirRecursively(m_tmpFolder);
}

bool GeoDataRiverSurveyWebImporter::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	return true;
}

bool GeoDataRiverSurveyWebImporter::importData(GeoData* data, int index, QWidget* w)
{
	return m_mlitImporter->importData(data, index, w);
}

void GeoDataRiverSurveyWebImporter::abortRequest()
{}

void GeoDataRiverSurveyWebImporter::handleDone()
{
	m_isWaitingHttpResponse = false;
}

bool GeoDataRiverSurveyWebImporter::doInit(int* count, SolverDefinitionGridAttribute* /*condition*/, PreProcessorGeoDataGroupDataItemI* item, QWidget* w)
{
	SettingDialog dialog(w);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return false;}

	m_setting = dialog.setting();
	*count = 1;

	const auto& s = m_setting;

	QUrl url("https://rivopen.ricplace2.org/api/download/" + s.riverId + "/" + s.minKp + "/" + s.maxKp + "/" + s.year);

	QNetworkRequest request(url);
	m_webReply = m_webAccessManager->get(request);
	m_isWaitingHttpResponse = true;
	connect(m_webReply, &QNetworkReply::finished, this, &GeoDataRiverSurveyWebImporter::handleDone);
	qApp->processEvents();

	while (m_isWaitingHttpResponse) {
		qApp->thread()->wait(100);
		qApp->processEvents();
	}

	if (m_webReply->error() != QNetworkReply::NoError) {
		QMessageBox::information(w, "Error", m_webReply->errorString());
	}

	QByteArray zipContent = m_webReply->readAll();
	m_tmpFile = item->projectData()->tmpFileName();

	QFile tmpZipFile(m_tmpFile);
	tmpZipFile.open(QFile::WriteOnly);
	tmpZipFile.write(zipContent);
	tmpZipFile.close();

	m_tmpFolder = item->projectData()->tmpFileName();
	iRIC::UnzipArchive(m_tmpFile, m_tmpFolder);

	QDir tmpDir(m_tmpFolder);

	auto fileNames = tmpDir.entryList(QDir::Filter::Files);
	QString mainCsvName;

	for (const auto& fileName : fileNames) {
		if (fileName.contains(".csv")) {
			mainCsvName = fileName;
		}
	}

	m_mlitImporter->importInit(tmpDir.absoluteFilePath(mainCsvName), tmpDir.absoluteFilePath("csv_files"), w);

	return true;
}
