#ifndef GEODATABACKGROUNDCELLMAPPERT_H
#define GEODATABACKGROUNDCELLMAPPERT_H

#include "../geodata/geodatacellmappert.h"
#include "../geodata/geodatamappersettingi.h"
#include "geodatabackground.h"
#include <misc/integerrangecontainer.h>

template <class V, class DA>
class GeoDataBackgroundCellMapperT : public GeoDataCellMapperT<V, DA>
{
public:
	GeoDataBackgroundCellMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatabackgroundcellmappert_detail.h"

#endif // GEODATABACKGROUNDCELLMAPPERT_H
