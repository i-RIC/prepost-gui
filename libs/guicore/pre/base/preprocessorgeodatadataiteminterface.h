#ifndef PREPROCESSORGEODATADATAITEMINTERFACE_H
#define PREPROCESSORGEODATADATAITEMINTERFACE_H

#include "preprocessordataitem.h"

class GeoData;

class PreProcessorGeoDataDataItemInterface : public PreProcessorDataItem
{

public:
	PreProcessorGeoDataDataItemInterface(const QString& itemlabel, const QIcon& icon, GraphicsWindowDataItem* parent)
		: PreProcessorDataItem(itemlabel, icon, parent)
	{}
	virtual GeoData* geoData() const = 0;
	virtual void setGeoData(GeoData* geoData) = 0;

	virtual void informValueRangeChange() = 0;
	virtual void informDataChange() = 0;
	virtual void setDeleteSilently(bool silent) = 0;

	friend class GeoData;
};

#endif // PREPROCESSORGEODATADATAITEMINTERFACE_H
