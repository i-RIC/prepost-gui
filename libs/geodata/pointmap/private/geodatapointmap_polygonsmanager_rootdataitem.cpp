#include "geodatapointmap_polygonsmanager_gridtypedataitem.h"
#include "geodatapointmap_polygonsmanager_rootdataitem.h"

#include <guicore/datamodel/graphicswindowdatamodel.h>

GeoDataPointmap::PolygonsManager::RootDataItem::RootDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataModel* model, PreProcessorDataItem* parent) :
	GraphicsWindowRootDataItem (nullptr, model),
	m_gridTypeDataItem {new GridTypeDataItem {cond, this, parent}}
{}

GeoDataPointmap::PolygonsManager::RootDataItem::~RootDataItem()
{
	delete m_gridTypeDataItem;
}

GeoDataPointmap::PolygonsManager::GridTypeDataItem* GeoDataPointmap::PolygonsManager::RootDataItem::gridTypeDataItem() const
{
	return m_gridTypeDataItem;
}
