#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_GEODATATOPDATAITEM_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_GEODATATOPDATAITEM_H

#include "geodatapointmap_polygonsmanager.h"

#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>

class GeoDataPointmap::PolygonsManager::GeoDataTopDataItem : public PreProcessorGeoDataTopDataItemInterface
{
public:
	GeoDataTopDataItem(SolverDefinitionGridAttribute* cond, GridTypeDataItem* gridTypeDataItem, PreProcessorDataItem* parent);

	GeoDataGroupDataItem* geoDataGroupDataItem() const;

	SolverDefinitionGridType* gridType() const override;
	const QList<PreProcessorGeoDataGroupDataItemInterface*> groupDataItems() const override;
	PreProcessorGeoDataGroupDataItemInterface* groupDataItem(const std::string& name) const override;
	void setDimensionsToFirst() override;
	void clearDimensionsIfNoDataExists() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GeoDataGroupDataItem* m_geoDataGroupDataItem;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_GEODATATOPDATAITEM_H
