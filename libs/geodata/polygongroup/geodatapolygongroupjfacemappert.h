#ifndef GEODATAPOLYGONGROUPJFACEMAPPERT_H
#define GEODATAPOLYGONGROUPJFACEMAPPERT_H

#include "geodatapolygongroup.h"

#include <guicore/pre/geodata/geodatajfacemappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/rangecontainert.h>

class GeoDataPolygonGroupJFaceMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolygonGroupJFaceMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPolygonGroupJFaceMapperSetting() override {}

	RangeContainerT<GeoDataPolygonGroupPolygon*> ranges;
};

template <class V, class DA>
class GeoDataPolygonGroupJFaceMapperT : public GeoDataJFaceMapperT<V, DA>
{
public:
	GeoDataPolygonGroupJFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapolygongroupjfacemappert_detail.h"

#endif // GEODATAPOLYGONGROUPJFACEMAPPERT_H
