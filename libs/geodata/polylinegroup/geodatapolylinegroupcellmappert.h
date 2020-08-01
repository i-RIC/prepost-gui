#ifndef GEODATAPOLYLINEGROUPCELLMAPPERT_H
#define GEODATAPOLYLINEGROUPCELLMAPPERT_H

#include "geodatapolylinegroup.h"

#include <guicore/pre/geodata/geodatacellmappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolyLineGroupCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolyLineGroupCellMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPolyLineGroupCellMapperSetting() override {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPolyLineGroupCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataPolyLineGroupCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapolylinegroupcellmappert_detail.h"

#endif // GEODATAPOLYLINEGROUPCELLMAPPERT_H
