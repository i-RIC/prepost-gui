#ifndef GEODATAPOINTMAPNODEMAPPER_H
#define GEODATAPOINTMAPNODEMAPPER_H

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/doublemappingsetting.h>

#include <vtkDoubleArray.h>

#include <vector>

class GeoDataPointmapNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointmapNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointmapNodeMapperSetting() {}

	std::vector<DoubleMappingSetting> settings;
};

class GeoDataPointmapNodeMapper : public GeoDataNodeMapperT<double, vtkDoubleArray>
{
public:
	GeoDataPointmapNodeMapper(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#endif // GEODATAPOINTMAPNODEMAPPER_H
