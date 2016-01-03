#ifndef GEODATAPOLYGONNODEMAPPERT_H
#define GEODATAPOLYGONNODEMAPPERT_H

#include "geodatapolygon.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolygonNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolygonNodeMapperSetting() : GeoDataMapperSettingI() {}
	virtual ~GeoDataPolygonNodeMapperSetting() {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPolygonNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataPolygonNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* setting) override;
};

#include "private/geodatapolygonnodemappert_detail.h"

#endif // GEODATAPOLYGONNODEMAPPERT_H
