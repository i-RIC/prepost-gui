#ifndef GEODATAPOINTGROUPCELLMAPPERT_H
#define GEODATAPOINTGROUPCELLMAPPERT_H

#include "geodatapointgroup.h"

#include <guicore/pre/geodata/geodatacellmappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/integerrangecontainer.h>

class GeoDataPointGroupCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointGroupCellMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointGroupCellMapperSetting() override {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPointGroupCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataPointGroupCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapointgroupcellmappert_detail.h"

#endif // GEODATAPOINTGROUPCELLMAPPERT_H
