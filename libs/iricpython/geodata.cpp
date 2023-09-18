#include "geodata.h"

iRICPython::GeoData::GeoData(PreProcessorGeoDataDataItemI* data) :
	m_geoDataItem {data}
{}

PreProcessorGeoDataDataItemI* iRICPython::GeoData::geoDataItem() const
{
	return m_geoDataItem;
}
