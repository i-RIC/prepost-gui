#include "geodatagdalgdalimporter_importersetting.h"

GeoDataGdalGdalImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	timeMode {"timeMode", false},
	csName {"csName"},
	fileNamePattern {"fileNamePattern"},
	timeZone {"timeZone"},
	fileNames {"fileNames", ""}
{}

void GeoDataGdalGdalImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	timeMode.load(node);
	csName.load(node);
	fileNamePattern.load(node);
	timeZone.load(node);
	fileNames.load(node);
}
void GeoDataGdalGdalImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	timeMode.save(writer);
	csName.save(writer);
	fileNamePattern.save(writer);
	timeZone.save(writer);
	fileNames.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataGdalGdalImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Coordinate System"), csName}));
	if (timeMode) {
		ret.push_back(Item({tr("Filename pattern"), fileNamePattern}));
		ret.push_back(Item({tr("Time Zone"), timeZone}));
		auto files = fileNames.value();
		files = files.replace(":", ", ");
		ret.push_back(Item({tr("File Names"), files}));
	}

	return ret;
}
