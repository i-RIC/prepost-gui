#include "geodatagdalgrayscalepngrealimporter_importersetting.h"

GeoDataGdalGrayscalePngRealImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	csName {"csName"}
{}

void GeoDataGdalGrayscalePngRealImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	csName.load(node);
}

void GeoDataGdalGrayscalePngRealImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	csName.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataGdalGrayscalePngRealImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Coordinate System"), csName}));

	return ret;
}
