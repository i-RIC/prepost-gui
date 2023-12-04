#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_GEODATATOPDATAITEM_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_GEODATATOPDATAITEM_H

#include "geodatapointmap_polygonsmanager.h"

#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>

class GeoDataPointmap::PolygonsManager::GeoDataTopDataItem : public PreProcessorGeoDataTopDataItemI
{
public:
	GeoDataTopDataItem(SolverDefinitionGridAttribute* cond, GridTypeDataItem* gridTypeDataItem, PreProcessorDataItem* parent);

	GeoDataGroupDataItem* geoDataGroupDataItem() const;

	SolverDefinitionGridType* gridType() const override;
	const QList<PreProcessorGeoDataGroupDataItemI*> groupDataItems() const override;
	PreProcessorGeoDataGroupDataItemI* groupDataItem(const std::string& name) const override;
	void setDimensionsToFirst() override;
	void clearDimensionsIfNoDataExists() override;

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	GeoDataGroupDataItem* m_geoDataGroupDataItem;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_GEODATATOPDATAITEM_H
