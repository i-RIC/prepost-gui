#ifndef GEODATAPOINTMAPCELLMAPPER_H
#define GEODATAPOINTMAPCELLMAPPER_H

#include "geodatapointmap.h"

#include <guicore/pre/geodata/geodatacellmappert.h>
#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <misc/doublemappingsetting.h>

#include <vector>

class GeoDataPointmapCellMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointmapCellMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointmapCellMapperSetting() {}

	std::vector<DoubleMappingSetting> settings;
};

class GeoDataPointmapCellMapper : public GeoDataCellMapperT<double, vtkDoubleArray>
{
public:
	GeoDataPointmapCellMapper(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATAPOINTMAPCELLMAPPER_H
