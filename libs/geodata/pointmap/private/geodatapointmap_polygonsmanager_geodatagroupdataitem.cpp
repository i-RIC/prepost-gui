#include "geodatapointmap_polygonsmanager_geodatadataitem.h"
#include "geodatapointmap_polygonsmanager_geodatagroupdataitem.h"
#include "geodatapointmap_polygonsmanager_geodatatopdataitem.h"

GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::GeoDataGroupDataItem(SolverDefinitionGridAttribute* cond, GeoDataTopDataItem* topDataItem, PreProcessorDataItem* parent) :
	PreProcessorGeoDataGroupDataItemI(cond, topDataItem),
	m_condition {cond},
	m_geoDataDataItem {new GeoDataDataItem(this, parent)}
{}

GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::~GeoDataGroupDataItem()
{
	delete m_geoDataDataItem;
}

GeoDataPointmap::PolygonsManager::GeoDataDataItem* GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::geoDataDataItem() const
{
	return m_geoDataDataItem;
}

ProjectData* GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::projectData() const
{
	return ProjectDataItem::projectData();
}

SolverDefinitionGridAttribute* GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::condition()
{
	return m_condition;
}

bool GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::getValueRange(double* /*min*/, double* /*max*/)
{
	return false;
}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::applyColorMapSetting()
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::setupStringConverter(GridAttributeStringConverter* /*converter*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::setupEditWidget(GridAttributeEditWidget* /*widget*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::addCopyPolygon(GeoDataPolygon* /*polygon*/)
{}

const QList<PreProcessorGeoDataDataItemI*> GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::geoDatas() const
{
	QList<PreProcessorGeoDataDataItemI*> empty;
	return empty;
}

GridAttributeDimensionsContainer* GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::dimensions() const
{
	return nullptr;
}

PreProcessorGeoDataDataItemI* GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::buildGeoDataDataItem()
{
	return nullptr;
}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::addGeoData(PreProcessorGeoDataDataItemI*/*geoData*/)
{}

GeoDataCreator* GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::getPointMapCreator()
{
	return nullptr;
}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::openCrossSectionWindow(GeoDataRiverSurvey* /*rs*/, const QString& /*crosssection*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::toggleCrosssectionWindowsGridCreatingMode(bool /*gridMode*/, GeoDataRiverSurvey* /*rs*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::informCtrlPointUpdateToCrosssectionWindows()
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::requestCrosssectionWindowDelete(GeoDataRiverSurveyCrosssectionWindowProjectDataItem* /*item*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::updateCrossectionWindows()
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void GeoDataPointmap::PolygonsManager::GeoDataGroupDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
