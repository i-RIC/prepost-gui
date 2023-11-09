#ifndef GEODATABACKGROUNDIFACEMAPPERT_H
#define GEODATABACKGROUNDIFACEMAPPERT_H

#include "../geodata/geodataifacemappert.h"
#include "../geodata/geodatamappersettingi.h"
#include "geodatabackground.h"
#include <misc/integerrangecontainer.h>

template <class V, class DA>
class GeoDataBackgroundIFaceMapperT : public GeoDataIFaceMapperT<V, DA>
{
public:
	GeoDataBackgroundIFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatabackgroundifacemappert_detail.h"

#endif // GEODATABACKGROUNDIFACEMAPPERT_H
