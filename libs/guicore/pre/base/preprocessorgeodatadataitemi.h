#ifndef PREPROCESSORGEODATADATAITEMI_H
#define PREPROCESSORGEODATADATAITEMI_H

#include "preprocessordataitem.h"

class ColorMapSettingContainerI;
class GeoData;
class PreProcessorGeoDataGroupDataItemI;

class PreProcessorGeoDataDataItemI : public PreProcessorDataItem
{
public:
	PreProcessorGeoDataDataItemI(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent) :
		PreProcessorDataItem(itemlabel, icon, parent)
	{}

	virtual PreProcessorGeoDataGroupDataItemI* groupDataItem() const = 0;
	virtual GeoData* geoData() const = 0;
	virtual void setGeoData(GeoData* geoData) = 0;

	virtual void informValueRangeChange() = 0;
	virtual void informDataChange() = 0;
	virtual ColorMapSettingContainerI* colorMapSettingContainer() const = 0;
	virtual void setDeleteSilently(bool silent) = 0;

	friend class GeoData;
};

#endif // PREPROCESSORGEODATADATAITEMI_H
