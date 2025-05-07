#include "geodatariversurveymlitimporter_importersetting.h"

GeoDataRiverSurveyMlitImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	cpSetting {"cpSetting", GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting::cpMiddle},
	csvFilename {"csvFilename"}
{}

void GeoDataRiverSurveyMlitImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	cpSetting.load(node);
	csvFilename.load(node);
}

void GeoDataRiverSurveyMlitImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	cpSetting.save(writer);
	csvFilename.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataRiverSurveyMlitImporter::ImporterSetting::customItems() const
{
	QString cpSettingStr;
	QString csvStr;

	if (cpSetting.value() == GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting::cpMiddle) {
		cpSettingStr = tr("Middle point of left bank and right bank");
	} else if (cpSetting.value() == GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting::cpElevation) {
		cpSettingStr = tr("Where the elevation is the smallest");
	} else {
		cpSettingStr = tr("Import center line from CSV file");
		csvStr = csvFilename;
	}

	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Where to define River center point"), cpSettingStr}));
	ret.push_back(Item({tr("CSV file name"), csvStr}));

	return ret;
}
