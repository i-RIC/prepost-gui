#include "geodatapointmap_polygonsmanager_geodatatopdataitem.h"
#include "geodatapointmap_polygonsmanager_gridtypedataitem.h"
#include "geodatapointmap_polygonsmanager_rootdataitem.h"

GeoDataPointmap::PolygonsManager::GridTypeDataItem::GridTypeDataItem(SolverDefinitionGridAttribute* cond, RootDataItem* rootDataItem, PreProcessorDataItem* parent) :
	PreProcessorGridTypeDataItemI("", QIcon(), rootDataItem),
	m_geoDataTopDataItem {new GeoDataTopDataItem {cond, this, parent}}
{
	addChildItem(m_geoDataTopDataItem);
}

GeoDataPointmap::PolygonsManager::GeoDataTopDataItem* GeoDataPointmap::PolygonsManager::GridTypeDataItem::geoDataTopDataItem() const
{
	return m_geoDataTopDataItem;
}

ColorMapSettingContainerI* GeoDataPointmap::PolygonsManager::GridTypeDataItem::colorMapSetting(const std::string& /*attName*/) const
{
	return nullptr;
}

ModifyCommandDialog* GeoDataPointmap::PolygonsManager::GridTypeDataItem::createApplyColorMapSettingDialog(const std::string& name, QWidget *parent)
{
	return nullptr;
}

SolverDefinitionGridType* GeoDataPointmap::PolygonsManager::GridTypeDataItem::gridType() const
{
	return nullptr;
}

const QList<PreProcessorGridAndGridCreatingConditionDataItemI*>& GeoDataPointmap::PolygonsManager::GridTypeDataItem::conditions() const
{
	QList<PreProcessorGridAndGridCreatingConditionDataItemI*> empty;
	return empty;
}

PreProcessorGeoDataTopDataItemI* GeoDataPointmap::PolygonsManager::GridTypeDataItem::geoDataTop() const
{
	return nullptr;
}

PreProcessorHydraulicDataTopDataItemI* GeoDataPointmap::PolygonsManager::GridTypeDataItem::hydraulicDataTop() const
{
	return nullptr;
}

void GeoDataPointmap::PolygonsManager::GridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void GeoDataPointmap::PolygonsManager::GridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
