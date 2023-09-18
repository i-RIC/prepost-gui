#ifndef GEODATABACKGROUNDJFACEMAPPERT_H
#define GEODATABACKGROUNDJFACEMAPPERT_H

#include "../geodata/geodatajfacemappert.h"
#include "../geodata/geodatamappersettingi.h"
#include "geodatabackground.h"
#include <misc/integerrangecontainer.h>

template <class V, class DA>
class GeoDataBackgroundJFaceMapperT : public GeoDataJFaceMapperT<V, DA>
{
public:
	GeoDataBackgroundJFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatabackgroundjfacemappert_detail.h"

#endif // GEODATABACKGROUNDJFACEMAPPERT_H
