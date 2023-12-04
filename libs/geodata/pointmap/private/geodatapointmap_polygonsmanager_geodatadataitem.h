#ifndef GEODATAPOINTMAP_POLYGONSMANAGER_GEODATADATAITEM_H
#define GEODATAPOINTMAP_POLYGONSMANAGER_GEODATADATAITEM_H

#include "geodatapointmap_polygonsmanager.h"

#include <guicore/pre/base/preprocessorgeodatadataitemi.h>

class GeoDataPointmap::PolygonsManager::GeoDataDataItem : public PreProcessorGeoDataDataItemI
{
public:
	GeoDataDataItem(GeoDataGroupDataItem* group, PreProcessorDataItem* parent);
	~GeoDataDataItem();

	PreProcessorGeoDataGroupDataItemI* groupDataItem() const override;
	GeoData* geoData() const override;
	void setGeoData(GeoData* geoData) override;

	void informValueRangeChange() override;
	void informDataChange() override;
	ColorMapSettingContainerI* colorMapSettingContainer() const override;
	void setColorMapSettingContainer(ColorMapSettingContainerI* cont);
	void setDeleteSilently(bool silent) override;
	bool customVisibility() const override;
	void setCustomVisibility(bool visible);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;

	ColorMapSettingContainerI* m_colorMapSettingContainer;
	GeoDataGroupDataItem* m_group;
	GeoDataPolygonGroup* m_geoData;
	bool m_customVisibility;
};

#endif // GEODATAPOINTMAP_POLYGONSMANAGER_GEODATADATAITEM_H
