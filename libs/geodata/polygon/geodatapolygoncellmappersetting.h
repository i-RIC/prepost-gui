#ifndef GEODATAPOLYGONCELLMAPPERSETTING_H
#define GEODATAPOLYGONCELLMAPPERSETTING_H

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolygonCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolygonCellMapperSetting();
	~GeoDataPolygonCellMapperSetting();

	IntegerRangeContainer ranges;
};

#endif // GEODATAPOLYGONCELLMAPPERSETTING_H
