#include "geodata.h"
#include "geodataimporter.h"
#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodataimporter.h>

iRICPython::GeoDataImporter::GeoDataImporter(::GeoDataImporter* importer, SolverDefinitionGridAttribute* att, PreProcessorGeoDataGroupDataItemInterface* groupItem) :
	m_importer {importer},
	m_att {att},
	m_groupItem {groupItem}
{}

iRICPython::GeoData* iRICPython::GeoDataImporter::importGeoData(const std::string& filename)
{
	auto geoData =  m_importer->import(filename.c_str(), "", m_att, m_groupItem, nullptr);
	return new iRICPython::GeoData(geoData);
}
