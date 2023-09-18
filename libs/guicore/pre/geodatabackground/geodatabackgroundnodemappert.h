#ifndef GEODATABACKGROUNDNODEMAPPERT_H
#define GEODATABACKGROUNDNODEMAPPERT_H

#include "../geodata/geodatamappersettingi.h"
#include "../geodata/geodatanodemappert.h"
#include "geodatabackground.h"
#include <misc/integerrangecontainer.h>

template <class V, class DA>
class GeoDataBackgroundNodeMapperT : public GeoDataNodeMapperT<V, DA>
{
public:
	GeoDataBackgroundNodeMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatabackgroundnodemappert_detail.h"

#endif // GEODATABACKGROUNDNODEMAPPERT_H
