#ifndef GEODATANETCDFJFACEMAPPERT_H
#define GEODATANETCDFJFACEMAPPERT_H

#include "geodatanetcdft.h"

#include <guicore/pre/geodata/geodatajfacemappert.h>

class GeoDataMapperSettingI;

template <class V, class DA>
class GeoDataNetcdfJFaceMapperT : public GeoDataJFaceMapperT<V, DA>
{
public:
	GeoDataNetcdfJFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatanetcdfjfacemappert_detail.h"

#endif // GEODATANETCDFJFACEMAPPERT_H
