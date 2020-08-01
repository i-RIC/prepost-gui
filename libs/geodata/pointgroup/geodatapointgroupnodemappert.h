#ifndef GEODATAPOINTGROUPNODEMAPPERT_H
#define GEODATAPOINTGROUPNODEMAPPERT_H

#include "geodatapointgroup.h"

#include <guicore/pre/geodata/geodatamappersettingi.h>
#include <guicore/pre/geodata/geodatanodemappert.h>
#include <misc/integerrangecontainer.h>

class GeoDataPointGroupNodeMapperSetting : public GeoDataMapperSettingI
{
public:
	GeoDataPointGroupNodeMapperSetting() : GeoDataMapperSettingI() {}
	~GeoDataPointGroupNodeMapperSetting() override {}
};

template <typename V, typename DA>
class GeoDataPointGroupNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataPointGroupNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* setting) override;
};

#include "private/geodatapointgroupnodemappert_detail.h"

#endif // GEODATAPOINTGROUPNODEMAPPERT_H
