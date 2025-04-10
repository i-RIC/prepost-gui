#include "geodatapointmaplandxmlimporter_importersetting.h"

GeoDataPointmapLandXmlImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	csName {"csName"}
{}

void GeoDataPointmapLandXmlImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	csName.load(node);
}

void GeoDataPointmapLandXmlImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	csName.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataPointmapLandXmlImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Coordinate System"), csName}));

	return ret;
}
