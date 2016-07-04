#include "geodata.h"

iRICPython::GeoData::GeoData(PreProcessorGeoDataDataItemInterface* data) :
	m_geoDataItem {data}
{}

PreProcessorGeoDataDataItemInterface* iRICPython::GeoData::geoDataItem() const
{
	return m_geoDataItem;
}
