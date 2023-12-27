#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_ROOTDATAITEM_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_ROOTDATAITEM_H

#include <guicore/datamodel/graphicswindowrootdataitem.h>

#include "geodatapointmap_polygonsmanager.h"

class PreProcessorDataItem;

class GeoDataPointmap::PolygonsManager::RootDataItem : public GraphicsWindowRootDataItem
{
public:
	RootDataItem(SolverDefinitionGridAttribute* cond, GraphicsWindowDataModel* model, PreProcessorDataItem* parent);
	~RootDataItem();

	GridTypeDataItem* gridTypeDataItem() const;

private:
	GridTypeDataItem* m_gridTypeDataItem;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_ROOTDATAITEM_H
