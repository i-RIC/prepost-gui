#include "geodatapointgroupcsvimporter_importersetting.h"

GeoDataPointGroupCsvImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	csName {"csName"}
{}

void GeoDataPointGroupCsvImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	csName.load(node);
}

void GeoDataPointGroupCsvImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	csName.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataPointGroupCsvImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Coordinate System"), csName}));

	return ret;
}
