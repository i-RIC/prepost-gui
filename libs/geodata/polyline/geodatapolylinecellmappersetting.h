#ifndef GEODATAPOLYLINECELLMAPPERSETTING_H
#define GEODATAPOLYLINECELLMAPPERSETTING_H

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/integerrangecontainer.h>

class GeoDataPolyLineCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPolyLineCellMapperSetting();
	~GeoDataPolyLineCellMapperSetting();

	IntegerRangeContainer ranges;
};

#endif // GEODATAPOLYLINECELLMAPPERSETTING_H
