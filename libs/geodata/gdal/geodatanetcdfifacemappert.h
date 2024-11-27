#ifndef GEODATANETCDFIFACEMAPPERT_H
#define GEODATANETCDFIFACEMAPPERT_H

#include "geodatanetcdft.h"

#include <guicore/pre/geodata/geodataifacemappert.h>

class GeoDataMapperSettingI;

template <class V, class DA>
class GeoDataNetcdfIFaceMapperT : public GeoDataIFaceMapperT<V, DA>
{
public:
	GeoDataNetcdfIFaceMapperT(GeoDataCreator* parent);

	GeoDataMapperSettingI* initialize(bool* boolMap) override;
	void map(bool* boolMap, GeoDataMapperSettingI* s) override;
	void terminate(GeoDataMapperSettingI* s) override;
};

#include "private/geodatanetcdfifacemappert_detail.h"

#endif // GEODATANETCDFIFACEMAPPERT_H
