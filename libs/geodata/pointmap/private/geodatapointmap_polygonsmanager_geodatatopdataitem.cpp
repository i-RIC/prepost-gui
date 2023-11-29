#include "geodatapointmap_polygonsmanager_geodatagroupdataitem.h"
#include "geodatapointmap_polygonsmanager_geodatatopdataitem.h"
#include "geodatapointmap_polygonsmanager_gridtypedataitem.h"

GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::GeoDataTopDataItem(SolverDefinitionGridAttribute* cond, GridTypeDataItem* gridTypeDataItem, PreProcessorDataItem* parent) :
	PreProcessorGeoDataTopDataItemInterface("", QIcon(), gridTypeDataItem),
	m_geoDataGroupDataItem {new GeoDataGroupDataItem(cond, this, parent)}
{
	addChildItem(m_geoDataGroupDataItem);
}

GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem* GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::geoDataGroupDataItem() const
{
	return m_geoDataGroupDataItem;
}

SolverDefinitionGridType* GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::gridType() const
{
	return nullptr;
}

const QList<PreProcessorGeoDataGroupDataItemInterface*> GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::groupDataItems() const
{
	QList<PreProcessorGeoDataGroupDataItemInterface*> empty;
	return empty;
}

PreProcessorGeoDataGroupDataItemInterface* GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::groupDataItem(const std::string& /*name*/) const
{
	return nullptr;
}

void GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::setDimensionsToFirst()
{}

void GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::clearDimensionsIfNoDataExists()
{}

void GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
