#include "geodata.h"
#include "geodatagroup.h"
#include "geodataimporter.h"

#include <pre/datamodel/preprocessorgeodatagroupdataitem.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

iRICPython::GeoDataGroup::GeoDataGroup(PreProcessorGeoDataGroupDataItem* item) :
	m_item {item}
{}

iRICPython::GeoDataGroup::~GeoDataGroup()
{}

std::string iRICPython::GeoDataGroup::name() const
{
	return m_item->condition()->name();
}

iRICPython::GeoDataImporter* iRICPython::GeoDataGroup::importer(const std::string& name) const
{
	auto imp = m_item->importer(name);
	return new iRICPython::GeoDataImporter(imp, m_item->condition(), m_item);
}

void iRICPython::GeoDataGroup::add(iRICPython::GeoData* data)
{
	m_item->addGeoData(data->geoDataItem());
}
