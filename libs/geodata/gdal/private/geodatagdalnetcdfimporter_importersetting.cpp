#include "geodatagdalnetcdfimporter_importersetting.h"

GeoDataGdalNetcdfImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {}
{}

void GeoDataGdalNetcdfImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{

}

void GeoDataGdalNetcdfImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{

}

std::vector<GeoDataImporterSetting::Item> GeoDataGdalNetcdfImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;


	return ret;
}
