#ifndef GEODATAPOLYGONGROUPNODEMAPPERT_H
#define GEODATAPOLYGONGROUPNODEMAPPERT_H

#include "geodatapolygongroup.h"
#include "geodatapolygongrouprangecontainer.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolygonGroupNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolygonGroupNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPolygonGroupNodeMapperSetting() override {}

	GeoDataPolygonGroupRangeContainer ranges;
};

template <typename V, typename DA>
class GeoDataPolygonGroupNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataPolygonGroupNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* setting) override;
};

#include "private/geodatapolygongroupnodemappert_detail.h"

#endif // GEODATAPOLYGONGROUPNODEMAPPERT_H
