#include "geodatapolygongroupcsvimporter_importersetting.h"

GeoDataPolygonGroupCsvImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	csName {"csName"}
{}

void GeoDataPolygonGroupCsvImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	csName.load(node);
}

void GeoDataPolygonGroupCsvImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	csName.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataPolygonGroupCsvImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Coordinate System"), csName}));

	return ret;
}
