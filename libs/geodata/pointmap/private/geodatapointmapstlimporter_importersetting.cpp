#include "geodatapointmapstlimporter_importersetting.h"

GeoDataPointmapSTLImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	csName {"csName"}
{}

void GeoDataPointmapSTLImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	csName.load(node);
}

void GeoDataPointmapSTLImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	csName.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataPointmapSTLImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Coordinate System"), csName}));

	return ret;
}
