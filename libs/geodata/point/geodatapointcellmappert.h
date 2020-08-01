#ifndef GEODATAPOINTCELLMAPPERT_H
#define GEODATAPOINTCELLMAPPERT_H

#include "geodatapoint.h"

#include <guicore/pre/geodata/geodatacellmappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/integerrangecontainer.h>

class GeoDataPointCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointCellMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointCellMapperSetting() {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPointCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataPointCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapointcellmappert_detail.h"

#endif // GEODATAPOINTCELLMAPPERT_H
