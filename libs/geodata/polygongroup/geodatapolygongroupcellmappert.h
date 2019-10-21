#ifndef GEODATAPOLYGONGROUPCELLMAPPERT_H
#define GEODATAPOLYGONGROUPCELLMAPPERT_H

#include "geodatapolygongroup.h"

#include <guicore/pre/geodata/geodatacellmappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolygonGroupCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolygonGroupCellMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPolygonGroupCellMapperSetting() override {}
	IntegerRangeContainer ranges;
};

template <class V, class DA>
class GeoDataPolygonGroupCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataPolygonGroupCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatapolygongroupcellmappert_detail.h"

#endif // GEODATAPOLYGONGROUPCELLMAPPERT_H
