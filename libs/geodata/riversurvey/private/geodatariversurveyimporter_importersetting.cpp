#include "geodatariversurveyimporter_importersetting.h"


GeoDataRiverSurveyImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	cpSetting {"cpSetting", GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting::cpMiddle},
	csvFilename {"csvFilename"},
	allNamesAreNumber {"allNamesAreNumber", true},
	reverseOrder {"reverseOrder", false}
{}

void GeoDataRiverSurveyImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	cpSetting.load(node);
	csvFilename.load(node);
	allNamesAreNumber.load(node);
	reverseOrder.load(node);
}

void GeoDataRiverSurveyImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	cpSetting.save(writer);
	csvFilename.save(writer);
	allNamesAreNumber.save(writer);
	reverseOrder.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataRiverSurveyImporter::ImporterSetting::customItems() const
{
	QString cpSettingStr;
	QString csvStr;
	QString reverseOrderStr;

	if (cpSetting.value() == GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting::cpMiddle) {
		cpSettingStr = tr("Middle point of left bank and right bank");
	} else if (cpSetting.value() == GeoDataRiverSurveyImporterSettingDialog::CenterPointSetting::cpElevation) {
		cpSettingStr = tr("Where the elevation is the smallest");
	} else {
		cpSettingStr = tr("Import center line from CSV file");
		csvStr = csvFilename;
	}

	if (reverseOrder) {
		reverseOrderStr = tr("Downstream to upstream");
	} else {
		reverseOrderStr = tr("Upstream to downstream");
	}

	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Where to define River center point"), cpSettingStr}));
	ret.push_back(Item({tr("CSV file name"), csvStr}));
	if (! allNamesAreNumber.value()) {
		ret.push_back(Item({tr("Order of cross sections"), reverseOrderStr}));
	}

	return ret;
}
