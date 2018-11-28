#ifndef GEODATAPOLYLINENODEMAPPERT_H
#define GEODATAPOLYLINENODEMAPPERT_H

#include "geodatapolyline.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolyLineNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolyLineNodeMapperSetting() : GeoDataMapperSettingI() {}
	virtual ~GeoDataPolyLineNodeMapperSetting() {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPolyLineNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataPolyLineNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* setting) override;
};

#include "private/geodatapolylinenodemappert_detail.h"

#endif // GEODATAPOLYLINENODEMAPPERT_H
