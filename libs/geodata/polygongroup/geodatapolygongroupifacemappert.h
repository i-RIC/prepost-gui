#ifndef GEODATAPOLYGONGROUPIFACEMAPPERT_H
#define GEODATAPOLYGONGROUPIFACEMAPPERT_H

#include "geodatapolygongroup.h"

#include <guicore/pre/geodata/geodataifacemappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/rangecontainert.h>

class GeoDataPolygonGroupIFaceMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolygonGroupIFaceMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPolygonGroupIFaceMapperSetting() override {}

	RangeContainerT<GeoDataPolygonGroupPolygon*> ranges;
};

template <class V, class DA>
class GeoDataPolygonGroupIFaceMapperT : public GeoDataIFaceMapperT<V, DA>
{
public:
	GeoDataPolygonGroupIFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapolygongroupifacemappert_detail.h"

#endif // GEODATAPOLYGONGROUPIFACEMAPPERT_H
