#include "geodatapointmap_polygonsmanager_geodatadataitem.h"
#include "geodatapointmap_polygonsmanager_geodatagroupdataitem.h"

#include <geodata/polygongroup/geodatapolygongroup.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>
#include <guicore/pre/geodata/geodatafactory.h>

GeoDataPointmap::PolygonsManager::GeoDataDataItem::GeoDataDataItem(GeoDataGroupDataItem* group, PreProcessorDataItem* parent) :
	PreProcessorGeoDataDataItemI(parent),
	m_colorMapSettingContainer {nullptr},
	m_geoData {nullptr},
	m_group {group},
	m_customVisibility {true}
{
	parent->addChildItem(this);
	m_geoData = new GeoDataPolygonGroup(this, GeoDataFactory::instance().creator("integerPolygonGroup"), group->condition());
}

GeoDataPointmap::PolygonsManager::GeoDataDataItem::~GeoDataDataItem()
{
	delete m_geoData;
}

PreProcessorGeoDataGroupDataItemI* GeoDataPointmap::PolygonsManager::GeoDataDataItem::groupDataItem() const
{
	return m_group;
}

GeoData* GeoDataPointmap::PolygonsManager::GeoDataDataItem::geoData() const
{
	return m_geoData;
}

void GeoDataPointmap::PolygonsManager::GeoDataDataItem::setGeoData(GeoData* /*geoData*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataDataItem::informValueRangeChange()
{}

void GeoDataPointmap::PolygonsManager::GeoDataDataItem::informDataChange()
{
	m_geoData->updateActorSetting();
	m_geoData->renderGraphicsView();
}

ColorMapSettingContainerI* GeoDataPointmap::PolygonsManager::GeoDataDataItem::colorMapSettingContainer() const
{
	return m_colorMapSettingContainer;
}

void GeoDataPointmap::PolygonsManager::GeoDataDataItem::setColorMapSettingContainer(ColorMapSettingContainerI* cont)
{
	m_colorMapSettingContainer = cont;
}

void GeoDataPointmap::PolygonsManager::GeoDataDataItem::setDeleteSilently(bool /*silent*/)
{}

bool GeoDataPointmap::PolygonsManager::GeoDataDataItem::customVisibility() const
{
	if (! m_customVisibility) {return false;}
	if (m_geoData == nullptr) {return false;}

	return (m_geoData->allData().size() > 0);
}

void GeoDataPointmap::PolygonsManager::GeoDataDataItem::setCustomVisibility(bool visible)
{
	m_customVisibility = visible;
}

void GeoDataPointmap::PolygonsManager::GeoDataDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
