#ifndef GEODATAPOINTNODEMAPPERT_H
#define GEODATAPOINTNODEMAPPERT_H

#include "geodatapoint.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/integerrangecontainer.h>

class GeoDataPointNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointNodeMapperSetting() : GeoDataMapperSettingI() {}
	virtual ~GeoDataPointNodeMapperSetting() {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPointNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataPointNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* setting) override;
};

#include "private/geodatapointnodemappert_detail.h"

#endif // GEODATAPOINTNODEMAPPERT_H
