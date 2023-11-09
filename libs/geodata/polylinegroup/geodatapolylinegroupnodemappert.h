#ifndef GEODATAPOLYLINEGROUPNODEMAPPERT_H
#define GEODATAPOLYLINEGROUPNODEMAPPERT_H

#include "geodatapolylinegroup.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/rangecontainert.h>

class GeoDataPolyLineGroupNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolyLineGroupNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPolyLineGroupNodeMapperSetting() override {}

	RangeContainerT<GeoDataPolyLineGroupPolyLine*> ranges;
};

template <typename V, typename DA>
class GeoDataPolyLineGroupNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataPolyLineGroupNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* setting) override;
};

#include "private/geodatapolylinegroupnodemappert_detail.h"

#endif // GEODATAPOLYLINEGROUPNODEMAPPERT_H
