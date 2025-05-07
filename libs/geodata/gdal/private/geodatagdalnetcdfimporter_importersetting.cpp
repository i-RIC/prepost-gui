#include "geodatagdalnetcdfimporter_importersetting.h"

GeoDataGdalNetcdfImporter::ImporterSetting::ImporterSetting() :
	GeoDataImporterSetting {},
	csName {"csName"},
	valueVariable {"valueVariable"},
	dims {"dims"}
{}

void GeoDataGdalNetcdfImporter::ImporterSetting::doLoadFromProjectMainFile(const QDomNode& node)
{
	csName.load(node);
	valueVariable.load(node);
	dims.load(node);
}

void GeoDataGdalNetcdfImporter::ImporterSetting::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	csName.save(writer);
	valueVariable.save(writer);
	dims.save(writer);
}

std::vector<GeoDataImporterSetting::Item> GeoDataGdalNetcdfImporter::ImporterSetting::customItems() const
{
	std::vector<GeoDataImporterSetting::Item> ret;

	ret.push_back(Item({tr("Coordinate System"), csName}));

	return ret;
}
